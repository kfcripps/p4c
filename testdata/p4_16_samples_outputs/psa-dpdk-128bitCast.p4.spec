
struct ethernet_t {
	bit<48> dst_addr
	bit<48> src_addr
	bit<16> ether_type
}

struct dpdk_pseudo_header_t {
	bit<64> pseudo
}

struct _p4c_tmp128_t {
	bit<64> inter
}

struct _p4c_sandbox_header_t {
	bit<64> upper_half
	bit<64> lower_half
}

struct psa_ingress_output_metadata_t {
	bit<8> class_of_service
	bit<8> clone
	bit<16> clone_session_id
	bit<8> drop
	bit<8> resubmit
	bit<32> multicast_group
	bit<32> egress_port
}

struct psa_egress_output_metadata_t {
	bit<8> clone
	bit<16> clone_session_id
	bit<8> drop
}

struct psa_egress_deparser_input_metadata_t {
	bit<32> egress_port
}

header ethernet instanceof ethernet_t
header dpdk_pseudo_header instanceof dpdk_pseudo_header_t

struct user_meta_data_t {
	bit<32> psa_ingress_input_metadata_ingress_port
	bit<8> psa_ingress_output_metadata_drop
	bit<32> psa_ingress_output_metadata_egress_port
	bit<48> local_metadata_addr
	bit<128> Ingress_tmp
	bit<128> Ingress_tmp_0
	bit<128> Ingress_tmp_1
	bit<128> Ingress_flg
}
metadata instanceof user_meta_data_t

header Ingress_tmp_128 instanceof _p4c_sandbox_header_t
header Ingress_tmp_tmp instanceof _p4c_tmp128_t
header Ingress_tmp_0_128 instanceof _p4c_sandbox_header_t
header Ingress_tmp_0_tmp instanceof _p4c_tmp128_t
header Ingress_tmp_1_128 instanceof _p4c_sandbox_header_t
header Ingress_tmp_1_tmp instanceof _p4c_tmp128_t
action NoAction args none {
	return
}

action macswp args none {
	mov m.Ingress_tmp m.Ingress_flg
	movh h.Ingress_tmp_128.upper_half m.Ingress_tmp
	mov h.Ingress_tmp_128.lower_half m.Ingress_tmp
	mov h.Ingress_tmp_tmp.inter h.Ingress_tmp_128.lower_half
	and h.Ingress_tmp_tmp.inter 0xFFFFFFFFFFFFFFFF
	mov m.Ingress_tmp h.Ingress_tmp_tmp.inter
	mov h.Ingress_tmp_tmp.inter h.Ingress_tmp_128.upper_half
	and h.Ingress_tmp_tmp.inter 0x0
	movh m.Ingress_tmp h.Ingress_tmp_tmp.inter
	mov m.Ingress_tmp_0 m.Ingress_tmp
	movh h.Ingress_tmp_0_128.upper_half m.Ingress_tmp_0
	mov h.Ingress_tmp_0_128.lower_half m.Ingress_tmp_0
	mov h.Ingress_tmp_0_tmp.inter h.Ingress_tmp_0_128.lower_half
	and h.Ingress_tmp_0_tmp.inter 0xFFFFFFFFFFFFFFFF
	mov m.Ingress_tmp_0 h.Ingress_tmp_0_tmp.inter
	mov h.Ingress_tmp_0_tmp.inter h.Ingress_tmp_0_128.upper_half
	and h.Ingress_tmp_0_tmp.inter 0x0
	movh m.Ingress_tmp_0 h.Ingress_tmp_0_tmp.inter
	mov m.Ingress_tmp_1 m.Ingress_tmp_0
	movh h.Ingress_tmp_1_128.upper_half m.Ingress_tmp_1
	mov h.Ingress_tmp_1_128.lower_half m.Ingress_tmp_1
	mov h.Ingress_tmp_1_tmp.inter h.Ingress_tmp_1_128.lower_half
	and h.Ingress_tmp_1_tmp.inter 0xFFFFFFFFFFFFFFFF
	mov m.Ingress_tmp_1 h.Ingress_tmp_1_tmp.inter
	mov h.Ingress_tmp_1_tmp.inter h.Ingress_tmp_1_128.upper_half
	and h.Ingress_tmp_1_tmp.inter 0x0
	movh m.Ingress_tmp_1 h.Ingress_tmp_1_tmp.inter
	mov h.dpdk_pseudo_header.pseudo m.Ingress_tmp_1
	jmpneq LABEL_END h.dpdk_pseudo_header.pseudo 0x2
	mov m.local_metadata_addr h.ethernet.dst_addr
	mov h.ethernet.dst_addr h.ethernet.src_addr
	mov h.ethernet.src_addr m.local_metadata_addr
	LABEL_END :	return
}

table stub {
	actions {
		macswp
		NoAction @defaultonly
	}
	default_action NoAction args none 
	size 0xF4240
}


apply {
	rx m.psa_ingress_input_metadata_ingress_port
	mov m.psa_ingress_output_metadata_drop 0x1
	extract h.ethernet
	mov m.psa_ingress_output_metadata_egress_port m.psa_ingress_input_metadata_ingress_port
	xor m.psa_ingress_output_metadata_egress_port 0x1
	table stub
	jmpneq LABEL_DROP m.psa_ingress_output_metadata_drop 0x0
	emit h.ethernet
	tx m.psa_ingress_output_metadata_egress_port
	LABEL_DROP :	drop
}


