/*
Copyright 2022-present Orange
Copyright 2022-present Open Networking Foundation

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#include "ebpfPipeline.h"

#include "backends/ebpf/ebpfParser.h"

namespace P4::EBPF {

bool EBPFPipeline::isEmpty() const {
    // Check if parser doesn't have any state
    // Why 3? Parser will always have at least start, accept and reject states.
    if (parser->parserBlock->container->states.size() > 3) {
        return false;
    }

    auto startState = parser->parserBlock->container->states.at(0);
    auto path = startState->selectExpression->to<IR::PathExpression>();
    if (!path) return false;
    if (!startState->components.empty() || path->path->name.name != IR::ParserState::accept) {
        return false;
    }

    // Check if control is empty
    if (!control->controlBlock->container->body->components.empty()) {
        return false;
    }

    // Check if deparser doesn't emit anything
    if (!deparser->controlBlock->container->body->components.empty()) {
        return false;
    }

    return true;
}

void EBPFPipeline::emitLocalVariables(CodeBuilder *builder) {
    builder->emitIndent();
    builder->appendFormat("%s %s = %s;", errorEnum.c_str(), errorVar.c_str(),
                          P4::P4CoreLibrary::instance().noError.str());
    builder->newline();
    builder->emitIndent();
    builder->appendFormat("void* %s = %s;", packetStartVar.c_str(),
                          builder->target->dataOffset(model.CPacketName.toString()).c_str());
    builder->newline();
    builder->emitIndent();
    builder->appendFormat("u8* %s = %s;", headerStartVar.c_str(), packetStartVar.c_str());
    builder->newline();
    builder->emitIndent();
    builder->appendFormat("void* %s = %s;", packetEndVar.c_str(),
                          builder->target->dataEnd(model.CPacketName.toString()).c_str());
    builder->newline();
    builder->emitIndent();
    builder->appendFormat("u32 %s = 0;", zeroKey.c_str());
    builder->newline();
    builder->emitIndent();
    builder->appendFormat("u32 %s = 1;", oneKey.c_str());
    builder->newline();
    builder->emitIndent();
    builder->appendFormat("unsigned char %s;", byteVar.c_str());
    builder->newline();

    builder->emitIndent();
    builder->appendFormat("u32 %s = ", lengthVar.c_str());
    emitPacketLength(builder);
    builder->endOfStatement(true);

    if (shouldEmitTimestamp()) {
        builder->emitIndent();
        builder->appendFormat("u64 %s = ", timestampVar.c_str());
        emitTimestamp(builder);
        builder->endOfStatement(true);
    }

    builder->emitIndent();
    builder->appendFormat("u32 %s = %s;", inputPortVar.c_str(), ifindexVar.c_str());
    builder->newline();
    emitInputPortMapping(builder);
}

void EBPFPipeline::emitUserMetadataInstance(CodeBuilder *builder) {
    builder->emitIndent();
    auto user_md_type = typeMap->getType(control->user_metadata);
    if (user_md_type == nullptr) {
        ::P4::error(ErrorType::ERR_UNSUPPORTED_ON_TARGET, "cannot emit user metadata");
    }
    auto userMetadataType = EBPFTypeFactory::instance->create(user_md_type);
    userMetadataType->declare(builder, control->user_metadata->name.name, true);
    builder->endOfStatement(true);
}

void EBPFPipeline::emitLocalHeaderInstancesAsPointers(CodeBuilder *builder) {
    builder->emitIndent();
    builder->appendFormat("struct %v *%v;", parser->headerType->to<EBPFStructType>()->name,
                          parser->headers->name);
    builder->newline();
}

void EBPFPipeline::emitCPUMAPHeadersInitializers(CodeBuilder *builder) {
    builder->emitIndent();
    builder->appendLine("struct hdr_md *hdrMd;");
}

void EBPFPipeline::emitHeaderInstances(CodeBuilder *builder) {
    emitCPUMAPHeadersInitializers(builder);
    builder->emitIndent();
    parser->headerType->declare(builder, parser->headers->name.name, true);
    builder->endOfStatement(false);
}

void EBPFPipeline::emitCPUMAPLookup(CodeBuilder *builder) {
    builder->emitIndent();
    builder->target->emitTableLookup(builder, "hdr_md_cpumap"_cs, zeroKey, "hdrMd"_cs);
    builder->endOfStatement(true);
}

void EBPFPipeline::emitCPUMAPInitializers(CodeBuilder *builder) {
    emitCPUMAPLookup(builder);
    builder->emitIndent();
    builder->append("if (!hdrMd)");
    builder->newline();
    builder->emitIndent();
    builder->emitIndent();
    builder->appendFormat("return %v;", dropReturnCode());
    builder->newline();
    builder->emitIndent();
    builder->appendLine("__builtin_memset(hdrMd, 0, sizeof(struct hdr_md));");
}

void EBPFPipeline::emitHeadersFromCPUMAP(CodeBuilder *builder) {
    builder->emitIndent();
    builder->appendFormat("%v = &(hdrMd->cpumap_hdr);", parser->headers->name);
}

void EBPFPipeline::emitMetadataFromCPUMAP(CodeBuilder *builder) {
    builder->emitIndent();
    builder->appendFormat("%v = &(hdrMd->cpumap_usermeta);", control->user_metadata->name);
}

void EBPFPipeline::emitGlobalMetadataInitializer(CodeBuilder *builder) {
    builder->emitIndent();
    builder->appendFormat(
        "struct psa_global_metadata *%v = (struct psa_global_metadata *) skb->cb;",
        compilerGlobalMetadata);
    builder->newline();
}

void EBPFPipeline::emitPacketLength(CodeBuilder *builder) {
    if (this->is<XDPIngressPipeline>() || this->is<XDPEgressPipeline>()) {
        builder->appendFormat("%s->data_end - %s->data", this->contextVar.c_str(),
                              this->contextVar.c_str());
    } else {
        builder->appendFormat("%s->len", this->contextVar.c_str());
    }
}

void EBPFPipeline::emitTimestamp(CodeBuilder *builder) {
    builder->appendFormat("bpf_ktime_get_ns()");
}

void EBPFPipeline::emitInputPortMapping(CodeBuilder *builder) {
    builder->emitIndent();
    builder->appendFormat("if (%s == PSA_PORT_RECIRCULATE) ", inputPortVar.c_str());
    builder->blockStart();
    builder->emitIndent();
    // To be conformant with psa.p4, where PSA_PORT_RECIRCULATE is constant.
    builder->appendFormat("%s = P4C_PSA_PORT_RECIRCULATE", inputPortVar.c_str());
    builder->endOfStatement(true);
    builder->blockEnd(true);
}

// =====================EBPFIngressPipeline===========================
void EBPFIngressPipeline::emitSharedMetadataInitializer(CodeBuilder *builder) {
    auto type = EBPFTypeFactory::instance->create(this->deparser->resubmit_meta->type);
    type->declare(builder, deparser->resubmit_meta->name.name, false);
    builder->endOfStatement(true);
}

void EBPFIngressPipeline::emitPSAControlInputMetadata(CodeBuilder *builder) {
    builder->emitIndent();
    builder->appendFormat(
        "struct psa_ingress_input_metadata_t %v = {\n"
        "            .ingress_port = %v,\n"
        "            .packet_path = %v,\n"
        "            .parser_error = %v,\n"
        "    };",
        control->inputStandardMetadata->name, inputPortVar, packetPathVar, errorVar);
    builder->newline();
    if (shouldEmitTimestamp()) {
        builder->emitIndent();
        builder->appendFormat("%v.ingress_timestamp = %v", control->inputStandardMetadata->name,
                              timestampVar);
        builder->endOfStatement(true);
    }
}

void EBPFIngressPipeline::emitPSAControlOutputMetadata(CodeBuilder *builder) {
    builder->emitIndent();

    builder->appendFormat(
        "struct psa_ingress_output_metadata_t %v = {\n"
        "            .drop = true,\n"
        "    };",
        control->outputStandardMetadata->name);
    builder->newline();
    builder->newline();
}

void EBPFIngressPipeline::emit(CodeBuilder *builder) {
    cstring msgStr, varStr;

    // Firstly emit process() in-lined function and then the actual BPF section.
    builder->append("static __always_inline");
    builder->spc();
    // FIXME: use Target to generate metadata type
    builder->appendFormat(
        "int process(%v *%s, %v %v *%v, struct psa_ingress_output_metadata_t *%v, "
        "struct psa_global_metadata *%v, ",
        builder->target->packetDescriptorType(), model.CPacketName.str(),
        parser->headerType->to<EBPFStructType>()->kind,
        parser->headerType->to<EBPFStructType>()->name, parser->headers->name,
        control->outputStandardMetadata->name, compilerGlobalMetadata);

    auto type = EBPFTypeFactory::instance->create(deparser->resubmit_meta->type);
    type->declare(builder, deparser->resubmit_meta->name.name, true);
    builder->append(")");
    builder->newline();
    builder->blockStart();

    emitLocalVariables(builder);

    builder->newline();
    emitUserMetadataInstance(builder);

    emitCPUMAPHeadersInitializers(builder);
    builder->newline();
    emitCPUMAPInitializers(builder);
    builder->newline();
    emitHeadersFromCPUMAP(builder);
    builder->newline();
    emitMetadataFromCPUMAP(builder);
    builder->newline();

    msgStr = absl::StrFormat(
        "%v parser: parsing new packet, input_port=%%d, path=%%d, "
        "pkt_len=%%d",
        sectionName);
    varStr = absl::StrFormat("%v->packet_path", compilerGlobalMetadata);
    builder->target->emitTraceMessage(builder, msgStr.c_str(), 3, inputPortVar.c_str(), varStr,
                                      lengthVar.c_str());

    // PARSER
    parser->emit(builder);
    builder->newline();

    // CONTROL
    builder->emitIndent();
    builder->append(IR::ParserState::accept);
    builder->append(":");
    builder->spc();
    builder->blockStart();
    emitPSAControlInputMetadata(builder);
    msgStr = absl::StrFormat("%v control: packet processing started", sectionName);
    builder->target->emitTraceMessage(builder, msgStr.c_str());
    control->emit(builder);
    builder->blockEnd(true);
    msgStr = absl::StrFormat("%v control: packet processing finished", sectionName);
    builder->target->emitTraceMessage(builder, msgStr.c_str());

    // DEPARSER
    builder->emitIndent();
    builder->blockStart();
    msgStr = absl::StrFormat("%v deparser: packet deparsing started", sectionName);
    builder->target->emitTraceMessage(builder, msgStr.c_str());
    deparser->emit(builder);
    msgStr = absl::StrFormat("%v deparser: packet deparsing finished", sectionName);
    builder->target->emitTraceMessage(builder, msgStr.c_str());
    builder->blockEnd(true);

    builder->emitIndent();
    builder->appendFormat("return %d;", actUnspecCode);
    builder->newline();
    builder->blockEnd(true);
    builder->target->emitCodeSection(builder, sectionName);
    builder->emitIndent();
    builder->appendFormat("int %v(%v *%s)", functionName, builder->target->packetDescriptorType(),
                          model.CPacketName.str());
    builder->spc();

    builder->blockStart();

    emitGlobalMetadataInitializer(builder);

    emitPSAControlOutputMetadata(builder);

    builder->emitIndent();
    emitSharedMetadataInitializer(builder);

    emitHeaderInstances(builder);
    builder->newline();

    builder->emitIndent();
    builder->appendFormat("int ret = %d;", actUnspecCode);
    builder->newline();

    builder->emitIndent();
    builder->appendLine("#pragma clang loop unroll(disable)");
    builder->emitIndent();
    builder->appendFormat("for (int i = 0; i < %d; i++) ", maxResubmitDepth);
    builder->blockStart();
    builder->emitIndent();
    builder->appendFormat("%v.resubmit = 0;", control->outputStandardMetadata->name);
    builder->newline();
    builder->emitIndent();
    builder->append("ret = process(skb, ");

    builder->appendFormat("(%v %v *) %v, &%v, %v, &%v);",
                          parser->headerType->to<EBPFStructType>()->kind,
                          parser->headerType->to<EBPFStructType>()->name, parser->headers->name,
                          control->outputStandardMetadata->name, compilerGlobalMetadata,
                          deparser->resubmit_meta->name);

    builder->newline();
    builder->appendFormat(
        "        if (%v.drop == 1 || %v.resubmit == 0) {\n"
        "            break;\n"
        "        }\n",
        control->outputStandardMetadata->name, control->outputStandardMetadata->name);
    builder->blockEnd(true);

    builder->emitIndent();
    builder->appendFormat(
        "if (ret != %d) {\n"
        "        return ret;\n"
        "    }",
        actUnspecCode);
    builder->newline();

    this->emitTrafficManager(builder);

    builder->blockEnd(true);
}

// =====================EBPFEgressPipeline============================
void EBPFEgressPipeline::emitPSAControlInputMetadata(CodeBuilder *builder) {
    builder->emitIndent();
    builder->appendFormat(
        "struct psa_egress_input_metadata_t %v = {\n"
        "            .class_of_service = %v,\n"
        "            .egress_port = %v,\n"
        "            .packet_path = %v,\n"
        "            .instance = %v,\n"
        "            .parser_error = %v,\n"
        "        };",
        control->inputStandardMetadata->name, priorityVar, inputPortVar, packetPathVar,
        pktInstanceVar, errorVar);
    builder->newline();
    if (shouldEmitTimestamp()) {
        builder->emitIndent();
        builder->appendFormat("%v.egress_timestamp = %v", control->inputStandardMetadata->name,
                              timestampVar);
        builder->endOfStatement(true);
    }
}

void EBPFEgressPipeline::emitPSAControlOutputMetadata(CodeBuilder *builder) {
    builder->emitIndent();
    builder->appendFormat("struct psa_egress_output_metadata_t %v = {\n",
                          control->outputStandardMetadata->name);
    builder->appendLine(
        "       .clone = false,\n"
        "            .drop = false,\n"
        "        };");

    builder->newline();
}

void EBPFEgressPipeline::emitCPUMAPLookup(CodeBuilder *builder) {
    builder->emitIndent();
    builder->target->emitTableLookup(builder, "hdr_md_cpumap"_cs, oneKey, "hdrMd"_cs);
    builder->endOfStatement(true);
}

void EBPFEgressPipeline::emit(CodeBuilder *builder) {
    cstring msgStr, varStr;

    builder->newline();
    progTarget->emitCodeSection(builder, sectionName);
    builder->emitIndent();
    progTarget->emitMain(builder, functionName, model.CPacketName.toString());
    builder->spc();
    builder->blockStart();

    emitGlobalMetadataInitializer(builder);
    emitCheckPacketMarkMetadata(builder);

    emitLocalVariables(builder);
    emitUserMetadataInstance(builder);
    builder->newline();

    emitHeaderInstances(builder);
    builder->newline();

    emitCPUMAPInitializers(builder);
    builder->newline();
    emitHeadersFromCPUMAP(builder);
    builder->newline();
    emitMetadataFromCPUMAP(builder);
    builder->newline();

    emitPSAControlOutputMetadata(builder);
    emitPSAControlInputMetadata(builder);

    msgStr = absl::StrFormat(
        "%v parser: parsing new packet, input_port=%%d, path=%%d, "
        "pkt_len=%%d",
        sectionName);
    varStr = absl::StrFormat("%v->packet_path", compilerGlobalMetadata);
    builder->target->emitTraceMessage(builder, msgStr.c_str(), 3, inputPortVar.c_str(), varStr,
                                      lengthVar.c_str());

    // PARSER
    parser->emit(builder);
    builder->emitIndent();

    // CONTROL
    builder->append(IR::ParserState::accept);
    builder->append(":");
    builder->newline();
    builder->emitIndent();
    builder->appendFormat("%v.parser_error = %v", control->inputStandardMetadata->name, errorVar);
    builder->endOfStatement(true);
    builder->newline();
    builder->emitIndent();
    builder->blockStart();
    builder->newline();
    msgStr = absl::StrFormat("%v control: packet processing started", sectionName);
    builder->target->emitTraceMessage(builder, msgStr.c_str());
    control->emit(builder);
    builder->blockEnd(true);
    msgStr = absl::StrFormat("%v control: packet processing finished", sectionName);
    builder->target->emitTraceMessage(builder, msgStr.c_str());

    // DEPARSER
    builder->emitIndent();
    builder->blockStart();
    msgStr = absl::StrFormat("%v deparser: packet deparsing started", sectionName);
    builder->target->emitTraceMessage(builder, msgStr.c_str());
    deparser->emit(builder);
    msgStr = absl::StrFormat("%v deparser: packet deparsing finished", sectionName);
    builder->target->emitTraceMessage(builder, msgStr.c_str());
    builder->blockEnd(true);

    this->emitTrafficManager(builder);
    builder->blockEnd(true);
}

// =====================TCIngressPipeline=============================
void TCIngressPipeline::emitGlobalMetadataInitializer(CodeBuilder *builder) {
    EBPFPipeline::emitGlobalMetadataInitializer(builder);

    // if Traffic Manager decided to pass packet to the kernel stack earlier, send it up immediately
    builder->emitIndent();
    builder->append("if (compiler_meta__->pass_to_kernel == true) return TC_ACT_OK;");
    builder->newline();

    // workaround to make TC protocol-independent, DO NOT REMOVE
    builder->emitIndent();
    // replace ether_type only if a packet comes from XDP
    builder->appendFormat("if (%v->packet_path == NORMAL) ", compilerGlobalMetadata);
    builder->blockStart();
    builder->emitIndent();
    builder->appendFormat("compiler_meta__->mark = %u", packetMark);
    builder->endOfStatement(true);
    builder->emitIndent();
    if (options.xdp2tcMode == XDP2TC_META) {
        emitTCWorkaroundUsingMeta(builder);
    } else if (options.xdp2tcMode == XDP2TC_HEAD) {
        emitTCWorkaroundUsingHead(builder);
    } else if (options.xdp2tcMode == XDP2TC_CPUMAP) {
        emitTCWorkaroundUsingCPUMAP(builder);
    } else {
        BUG("no xdp2tc mode specified?");
    }
    builder->blockEnd(true);
}

void TCIngressPipeline::emitTCWorkaroundUsingMeta(CodeBuilder *builder) {
    builder->append(
        "struct internal_metadata *md = "
        "(struct internal_metadata *)(unsigned long)skb->data_meta;\n"
        "        if ((void *) ((struct internal_metadata *) md + 1) <= "
        "(void *)(long)skb->data) {\n"
        "            __u16 *ether_type = (__u16 *) ((void *) (long)skb->data + 12);\n"
        "            if ((void *) ((__u16 *) ether_type + 1) > (void *) (long) skb->data_end) {\n"
        "                return TC_ACT_SHOT;\n"
        "            }\n"
        "            *ether_type = md->pkt_ether_type;\n"
        "        }\n");
}

void TCIngressPipeline::emitTCWorkaroundUsingHead(CodeBuilder *builder) {
    builder->append(
        "    void *data = (void *)(long)skb->data;\n"
        "    void *data_end = (void *)(long)skb->data_end;\n"
        "    __u16 *orig_ethtype = data + 14;\n"
        "    if ((void *)((__u16 *) orig_ethtype + 1) > data_end) {\n"
        "        return TC_ACT_SHOT;\n"
        "    }\n"
        "    __u16 original_ethtype = *orig_ethtype;\n"
        "    int ret = bpf_skb_adjust_room(skb, -2, 1, 0);\n"
        "    if (ret < 0) {\n"
        "        return TC_ACT_SHOT;\n"
        "    }\n"
        "    data = (void *)(long)skb->data;\n"
        "    data_end = (void *)(long)skb->data_end;\n"
        "    struct ethhdr *eth = data;\n"
        "    if ((void *)((struct ethhdr *) eth + 1) > data_end) {\n"
        "        return TC_ACT_SHOT;\n"
        "    }\n"
        "    eth->h_proto = original_ethtype;");
}

void TCIngressPipeline::emitTCWorkaroundUsingCPUMAP(CodeBuilder *builder) {
    builder->append(
        "    void *data = (void *)(long)skb->data;\n"
        "    void *data_end = (void *)(long)skb->data_end;\n"
        "    u32 zeroKey = 0;\n"
        "    u16 *orig_ethtype = BPF_MAP_LOOKUP_ELEM(xdp2tc_cpumap, &zeroKey);\n"
        "    if (!orig_ethtype) {\n"
        "        return TC_ACT_SHOT;\n"
        "    }\n"
        "    struct ethhdr *eth = data;\n"
        "    if ((void *)((struct ethhdr *) eth + 1) > data_end) {\n"
        "        return TC_ACT_SHOT;\n"
        "    }\n"
        "    eth->h_proto = *orig_ethtype;\n");
}

/// The Traffic Manager for Ingress pipeline implements:
/// - Multicast handling
/// - send to port
void TCIngressPipeline::emitTrafficManager(CodeBuilder *builder) {
    auto mcast_grp = absl::StrFormat("%v.multicast_group", control->outputStandardMetadata->name);
    builder->emitIndent();
    builder->appendFormat("if (%s != 0) ", mcast_grp);
    builder->blockStart();
    builder->target->emitTraceMessage(
        builder, "IngressTM: Performing multicast, multicast_group=%u", 1, mcast_grp.c_str());
    builder->emitIndent();
    builder->appendFormat("do_packet_clones(%s, &multicast_grp_tbl, %s, NORMAL_MULTICAST, 2)",
                          contextVar.c_str(), mcast_grp.c_str());
    builder->endOfStatement(true);
    // In multicast mode, unicast packet is not send
    builder->target->emitTraceMessage(builder, "IngressTM: Multicast done, dropping source packet");
    builder->emitIndent();
    builder->appendFormat("return %v", dropReturnCode());
    builder->endOfStatement(true);
    builder->blockEnd(true);

    builder->emitIndent();
    builder->appendFormat("skb->priority = %v.class_of_service;",
                          control->outputStandardMetadata->name);
    builder->newline();

    builder->emitIndent();
    builder->appendFormat("if (!%v.drop && %v.egress_port == 0) ",
                          control->outputStandardMetadata->name,
                          control->outputStandardMetadata->name);
    builder->blockStart();
    builder->target->emitTraceMessage(builder, "IngressTM: Sending packet up to the kernel stack");

    // Since XDP helper re-writes EtherType for packets other than IPv4 (e.g., ARP)
    // we cannot simply return TC_ACT_OK to pass the packet up to the kernel stack,
    // because the kernel stack would receive a malformed packet (with invalid skb->protocol).
    // The workaround is to send the packet back to the same interface. If we redirect,
    // the packet will be re-written back to the original format.
    // At the beginning of the pipeline we check if pass_to_kernel is true and,
    // if so, the program returns TC_ACT_OK.
    builder->emitIndent();
    builder->appendLine("compiler_meta__->pass_to_kernel = true;");
    builder->emitIndent();
    builder->append("return bpf_redirect(skb->ifindex, BPF_F_INGRESS)");
    builder->endOfStatement(true);
    builder->blockEnd(true);

    cstring eg_port = absl::StrFormat("%v.egress_port", control->outputStandardMetadata->name);
    cstring cos = absl::StrFormat("%v.class_of_service", control->outputStandardMetadata->name);
    builder->target->emitTraceMessage(
        builder, "IngressTM: Sending packet out of port %d with priority %d", 2, eg_port, cos);
    builder->emitIndent();
    builder->appendFormat("return bpf_redirect(%v.egress_port, 0)",
                          control->outputStandardMetadata->name);
    builder->endOfStatement(true);
}

// =====================TCEgressPipeline=============================
void TCEgressPipeline::emitTrafficManager(CodeBuilder *builder) {
    cstring varStr;
    // clone support
    builder->emitIndent();
    builder->appendFormat("if (%v.clone) ", control->outputStandardMetadata->name);
    builder->blockStart();

    builder->emitIndent();
    builder->appendFormat(
        "do_packet_clones(%v, &clone_session_tbl, %v.clone_session_id, "
        "CLONE_E2E, 3)",
        contextVar, control->outputStandardMetadata->name);
    builder->endOfStatement(true);
    builder->blockEnd(true);

    builder->newline();

    // drop support
    builder->emitIndent();
    builder->appendFormat("if (%v.drop) ", control->outputStandardMetadata->name);
    builder->blockStart();
    builder->target->emitTraceMessage(builder, "EgressTM: Packet dropped due to metadata");
    builder->emitIndent();
    builder->appendFormat("return %v;", dropReturnCode());
    builder->endOfStatement(true);
    builder->blockEnd(true);

    builder->newline();

    // recirculation support
    // TODO: pass recirculation metadata
    // TODO: there is parameter type `psa_egress_deparser_input_metadata_t` to the deparser,
    //  maybe it should be used instead of `istd`?
    builder->emitIndent();
    builder->appendFormat("if (%v.egress_port == P4C_PSA_PORT_RECIRCULATE) ",
                          control->inputStandardMetadata->name);
    builder->blockStart();
    builder->target->emitTraceMessage(builder, "EgressTM: recirculating packet");
    builder->emitIndent();
    builder->appendFormat("%v->packet_path = RECIRCULATE", compilerGlobalMetadata);
    builder->endOfStatement(true);
    builder->emitIndent();
    builder->appendFormat("return bpf_redirect(PSA_PORT_RECIRCULATE, BPF_F_INGRESS)");
    builder->endOfStatement(true);
    builder->blockEnd(true);

    builder->newline();

    // normal packet to port
    varStr = absl::StrFormat("%v->ifindex", contextVar);
    builder->target->emitTraceMessage(builder, "EgressTM: output packet to port %d", 1,
                                      varStr.c_str());
    builder->emitIndent();

    builder->newline();
    builder->emitIndent();
    builder->appendFormat("return %v", forwardReturnCode());
    builder->endOfStatement(true);
}

void TCEgressPipeline::emitCheckPacketMarkMetadata(CodeBuilder *builder) {
    builder->emitIndent();
    builder->appendFormat("if (compiler_meta__->mark != %u) ", packetMark);
    builder->blockStart();
    builder->emitIndent();
    builder->appendFormat("return %v", forwardReturnCode());
    builder->endOfStatement(true);
    builder->blockEnd(true);
}

// =====================XDPIngressPipeline=============================
void XDPIngressPipeline::emitGlobalMetadataInitializer(CodeBuilder *builder) {
    builder->emitIndent();
    builder->append("struct psa_global_metadata instance = {}");
    builder->endOfStatement(true);
    builder->emitIndent();
    builder->appendFormat("struct psa_global_metadata *%v = &instance;", compilerGlobalMetadata);
    builder->newline();
    builder->emitIndent();
    builder->appendFormat("%v->packet_path = NORMAL", compilerGlobalMetadata);
    builder->endOfStatement(true);
}

void XDPIngressPipeline::emitTrafficManager(CodeBuilder *builder) {
    // do not handle multicast; it has been handled earlier by PreDeparser.
    cstring portVar = absl::StrFormat("%v.egress_port", control->outputStandardMetadata->name);
    builder->target->emitTraceMessage(builder, "IngressTM: Sending packet out of port %u", 1,
                                      portVar);
    builder->emitIndent();
    builder->appendFormat("return bpf_redirect_map(&tx_port, %v.egress_port%s, 0);",
                          control->outputStandardMetadata->name, "%DEVMAP_SIZE");
    builder->newline();
}

// =====================XDPEgressPipeline=============================
void XDPEgressPipeline::emitGlobalMetadataInitializer(CodeBuilder *builder) {
    builder->emitIndent();
    builder->append("struct psa_global_metadata instance = {}");
    builder->endOfStatement(true);
    builder->emitIndent();
    builder->appendFormat("struct psa_global_metadata *%v = &instance;", compilerGlobalMetadata);
    builder->newline();
    builder->emitIndent();
    builder->appendFormat("%v->packet_path = NORMAL_UNICAST", compilerGlobalMetadata);
    builder->endOfStatement(true);
}

void XDPEgressPipeline::emitTrafficManager(CodeBuilder *builder) {
    cstring varStr;

    builder->newline();
    builder->emitIndent();
    builder->appendFormat("if (%v.clone || %v.drop) ", control->outputStandardMetadata->name,
                          control->outputStandardMetadata->name);
    builder->blockStart();
    builder->target->emitTraceMessage(builder, "EgressTM: Packet dropped due to metadata");
    builder->emitIndent();
    builder->appendFormat("return %v", dropReturnCode());
    builder->endOfStatement(true);
    builder->blockEnd(true);

    builder->newline();

    // normal packet to port
    varStr = absl::StrFormat("%v.egress_port", control->inputStandardMetadata->name);
    builder->target->emitTraceMessage(builder, "EgressTM: output packet to port %d", 1, varStr);
    builder->emitIndent();
    builder->appendFormat("return %v;", this->forwardReturnCode());
    builder->newline();
}

void XDPEgressPipeline::emitCheckPacketMarkMetadata(CodeBuilder *builder) {
    (void)builder;
    // Global metadata in XDP is not preserved across ingress and egress so do not check packet
    // mark. Anyway, in XDP egress can't be called without our ingress.
}

// =====================TCTrafficManagerForXDP=============================

void TCTrafficManagerForXDP::emitGlobalMetadataInitializer(CodeBuilder *builder) {
    EBPFPipeline::emitGlobalMetadataInitializer(builder);

    // if Traffic Manager decided to pass packet to the kernel stack earlier, send it up immediately
    builder->emitIndent();
    builder->appendFormat("if (%v->pass_to_kernel) return %v;", compilerGlobalMetadata,
                          progTarget->forwardReturnCode());
    builder->newline();

    // Mark packet for egress processing
    builder->emitIndent();
    builder->appendFormat("%v->mark = %u", compilerGlobalMetadata, packetMark);
    builder->endOfStatement(true);
}

void TCTrafficManagerForXDP::emit(CodeBuilder *builder) {
    cstring msgStr;
    progTarget->emitCodeSection(builder, sectionName);
    builder->emitIndent();
    progTarget->emitMain(builder, functionName, model.CPacketName.toString());
    builder->spc();
    builder->blockStart();
    emitGlobalMetadataInitializer(builder);
    emitLocalVariables(builder);

    if (options.xdp2tcMode == XDP2TC_CPUMAP) {
        emitReadXDP2TCMetadataFromCPUMAP(builder);
    } else if (options.xdp2tcMode == XDP2TC_HEAD) {
        emitReadXDP2TCMetadataFromHead(builder);
    }

    msgStr = absl::StrFormat("%v deparser: packet deparsing started", sectionName);
    builder->target->emitTraceMessage(builder, msgStr.c_str());
    builder->emitIndent();
    deparser->emit(builder);
    msgStr = absl::StrFormat("%v deparser: packet deparsing finished", sectionName);
    builder->target->emitTraceMessage(builder, msgStr.c_str());
    this->emitTrafficManager(builder);

    builder->blockEnd(true);
}

void TCTrafficManagerForXDP::emitReadXDP2TCMetadataFromHead(CodeBuilder *builder) {
    builder->emitIndent();
    builder->append(
        "    void *data = (void *)(long)skb->data;\n"
        "    void *data_end = (void *)(long)skb->data_end;\n"
        "    if (((char *) data + sizeof(struct ethhdr) + sizeof(struct xdp2tc_metadata)) > "
        "(char *) data_end) {\n"
        "        return TC_ACT_SHOT;\n"
        "    }\n");
    builder->emitIndent();
    builder->appendLine("struct xdp2tc_metadata xdp2tc_md = {};");
    builder->emitIndent();
    builder->appendFormat(
        "bpf_skb_load_bytes(%s, sizeof(struct ethhdr), &xdp2tc_md, "
        "sizeof(struct xdp2tc_metadata))",
        model.CPacketName.str());
    builder->endOfStatement(true);
    builder->emitIndent();
    builder->append(
        "    __u16 *ether_type = (__u16 *) ((void *) (long)skb->data + 12);\n"
        "    if ((void *) ((__u16 *) ether_type + 1) > "
        "    (void *) (long) skb->data_end) {\n"
        "        return TC_ACT_SHOT;\n"
        "    }\n"
        "    *ether_type = xdp2tc_md.pkt_ether_type;\n");
    builder->emitIndent();
    builder->appendFormat("struct psa_ingress_output_metadata_t %v = xdp2tc_md.ostd;",
                          control->outputStandardMetadata->name);
    builder->newline();

    builder->emitIndent();
    emitLocalHeaderInstancesAsPointers(builder);

    builder->emitIndent();
    builder->appendFormat("%v = &(xdp2tc_md.headers);", parser->headers->name);
    builder->newline();

    builder->emitIndent();
    builder->appendFormat("%s = (u8*)%s + BYTES(xdp2tc_md.packetOffsetInBits);",
                          headerStartVar.c_str(), packetStartVar.c_str());

    builder->newline();
    builder->emitIndent();
    builder->appendFormat("int ret = bpf_skb_adjust_room(%s, -(int)%s, 1, 0)",
                          model.CPacketName.str(), "sizeof(struct xdp2tc_metadata)");
    builder->endOfStatement(true);
    builder->emitIndent();
    builder->append("if (ret) ");
    builder->blockStart();
    builder->target->emitTraceMessage(
        builder, "Deparser: failed to remove XDP2TC metadata from packet, ret=%d", 1, "ret");
    builder->emitIndent();
    builder->appendFormat("return %s;", builder->target->abortReturnCode().c_str());
    builder->newline();
    builder->blockEnd(true);
}

void TCTrafficManagerForXDP::emitReadXDP2TCMetadataFromCPUMAP(CodeBuilder *builder) {
    builder->emitIndent();
    builder->target->emitTableLookup(builder, "xdp2tc_shared_map"_cs, this->zeroKey,
                                     "struct xdp2tc_metadata *md"_cs);
    builder->endOfStatement(true);
    builder->emitIndent();
    builder->append("if (!md) ");
    builder->blockStart();
    builder->appendFormat("return %v;", dropReturnCode());
    builder->newline();
    builder->blockEnd(true);
    builder->emitIndent();

    builder->emitIndent();
    emitLocalHeaderInstancesAsPointers(builder);
    builder->emitIndent();
    builder->appendFormat("%v = &(md->headers);", parser->headers->name);
    builder->newline();

    builder->emitIndent();
    builder->appendFormat("struct psa_ingress_output_metadata_t %v = md->ostd;",
                          this->control->outputStandardMetadata->name);
    builder->newline();
    builder->emitIndent();
    builder->appendFormat("%v = (u8*)%v + BYTES(md->packetOffsetInBits);", headerStartVar,
                          packetStartVar);

    builder->emitIndent();
    builder->append(
        "    __u16 *ether_type = (__u16 *) ((void *) (long)skb->data + 12);\n"
        "    if ((void *) ((__u16 *) ether_type + 1) > "
        "    (void *) (long) skb->data_end) {\n"
        "        return TC_ACT_SHOT;\n"
        "    }\n"
        "    *ether_type = md->pkt_ether_type;\n");

    builder->emitIndent();
}

}  // namespace P4::EBPF
