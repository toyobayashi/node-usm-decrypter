{
  "variables": {
    "module_name": "usm",
    "module_path": "./dist"
  },
  'target_defaults': {
    'defines': [
    ]
  },
  'targets': [
    {
      'target_name': '<(module_name)',
      'sources': [
        'src/index.cpp',
        'src/fopen.cpp',
        'src/clADX.cpp',
        'src/clCRID.cpp',
        'src/clUTF.cpp',
      ],
      'includes': [
        './common.gypi'
      ]
    },
    {
      "target_name": "action_after_build",
      "type": "none",
      "dependencies": [ "<(module_name)" ],
      "copies": [
        {
          "files": [ "<(PRODUCT_DIR)/<(module_name).node" ],
          "destination": "<(module_path)"
        }
      ]
    }
  ]
}
