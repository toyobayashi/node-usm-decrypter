{
  "variables": {
    "module_path": "./dist",
    "PRODUCT_DIR": "./build/Release"
  },
  'target_defaults': {
    'defines': [
    ]
  },
  'targets': [
    {
      'target_name': 'usm',
      'sources': [
        'src/index.cpp',
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
      "dependencies": [ "usm" ],
      "copies": [
        {
          "files": [ "<(PRODUCT_DIR)/usm.node" ],
          "destination": "<(module_path)"
        }
      ]
    }
  ]
}
