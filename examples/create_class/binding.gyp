{
    "targets": [
        {
            "target_name": "create_class",
            "sources": [
                "src/create_class.cpp"
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
