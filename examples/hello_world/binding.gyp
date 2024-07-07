{
    "targets": [
        {
            "target_name": "hello_world",
            "sources": [
                "src/hello_world.cpp"
            ],
            "include_dirs": [
                "../..",
                "../../_build/craftpp/include"
            ],
            "libraries": [
                "-L../../../_build/craftnapi",
                "-lcraftnapi",
                "-L../../../_build/craftpp/lib",
                "-lcraftpp",
            ],
            "cflags_cc": [ "-std=c++20" ],
            "xcode_settings": {
                "OTHER_CFLAGS": ["-std=c++20"],
            }
        }
    ]
}
