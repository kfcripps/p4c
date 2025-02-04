extern void __e(in bit<28> arg);
struct meta_t {
    bit<8> f1;
    bit<8> f2;
}

action foo(inout meta_t meta) {
    switch (meta.f1 + meta.f2) {
        8w0: {
            __e(28w0);
        }
        8w2: {
        }
        8w3: 
        8w5: {
            __e(28w5);
        }
        8w7: 
        default: {
            __e(28w99);
        }
    }
}
control C(inout meta_t meta) {
    table t {
        actions = {
            foo(meta);
        }
        default_action = foo(meta);
    }
    apply {
        t.apply();
    }
}

control proto(inout meta_t meta);
package top(proto p);
top(C()) main;
