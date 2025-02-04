extern void __e(in bit<28> arg);
struct meta_t {
    bit<8> f1;
    bit<8> f2;
}

control C(inout meta_t meta) {
    @name("C.meta") meta_t meta_0;
    @name(".foo") action foo_0() {
        meta_0 = meta;
        switch (meta_0.f1 + meta_0.f2) {
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
        meta = meta_0;
    }
    @name("C.t") table t_0 {
        actions = {
            foo_0();
        }
        default_action = foo_0();
    }
    apply {
        t_0.apply();
    }
}

control proto(inout meta_t meta);
package top(proto p);
top(C()) main;
