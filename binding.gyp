{
    "targets": [{
        "target_name": "thread",
        "cflags!": [ "-fno-exceptions" ],
        "cflags_cc!": [ "-fno-exceptions" ],
        "conditions": [
          ["OS=='win' and target_arch=='x64'", {
            "sources": [
              "src/thread.cpp",
              "src/string.cpp",
              "src/wow64.cpp"
            ]
          }],
          ["OS=='win'", {
            "sources": [
              "src/thread.cpp",
              "src/string.cpp"
            ]
          }]
        ],
        "msvs_settings": {
          "VCCLCompilerTool": {
            "ExceptionHandling": 1,
            "AdditionalOptions": []
          }
        },
        "include_dirs": [
            "<!(node -p \"require('node-addon-api').include_dir\")"
        ],
        "dependencies": [
            "<!(node -p \"require('node-addon-api').targets\"):node_addon_api_maybe"
        ],
        "defines": [ "NODE_ADDON_API_DISABLE_DEPRECATED" ]
    }]
}