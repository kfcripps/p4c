extern void __e(in bit<28> arg);
control C() {
    @name("C.x") bit<8> x_0;
    @name("C.foo") action foo() {
        x_0 = 8w1;
        switch (x_0) {
            8w0: {
                __e(28w2);
            }
            default: {
                __e(28w3);
            }
        }
    }
    @name("C.t") table t_0 {
        actions = {
            foo();
        }
        default_action = foo();
    }
    apply {
        t_0.apply();
    }
}

control proto();
package top(proto p);
top(C()) main;
