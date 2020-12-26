const { USMDecrypter } = require('../build/Debug/usm.node')
const fs = require('fs')

const path = require('path')
const p = path.join(__dirname, 'movie9909.usm')

fs.mkdirSync(path.join(path.dirname(p), path.parse(p).name + '.demux'), { recursive: true })

const usm = new USMDecrypter()
usm.demux(p, function (err, outdir) {
  console.log(err, outdir)
})

// console.log(USMDecrypter)
