extern void __e(in bit<28> arg);
control C() {
    action foo(in bit<8> x) {
        switch (x) {
            8w0: {
                __e(28w2);
            }
            default: {
                __e(28w3);
            }
        }
    }
    table t {
        actions = {
            foo(8w1);
        }
        default_action = foo(8w1);
    }
    apply {
        t.apply();
    }
}

control proto();
package top(proto p);
top(C()) main;
