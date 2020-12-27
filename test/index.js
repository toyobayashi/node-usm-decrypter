const { USMDecrypter, Encoding } = require('..')
const { Downloader, ResourceType } = require('mishiro-core')
const fs = require('fs-extra')
const path = require('path')
const assert = require('assert')

const getPath = (...args) => {
  return path.join(__dirname, '..', ...args)
}

const downloader = new Downloader()
const usm = new USMDecrypter()
console.log(usm.getEncoding())
console.log(Encoding)

const usmPath = getPath('./test/movie9909.usm')
const defaultDir = path.join(path.dirname(usmPath), path.parse(usmPath).name + '.demux')

function demuxAsync (...args) {
  return new Promise((resolve, reject) => {
    usm.demux(...args, function (err, p) {
      if (err) {
        reject(err)
        return
      }
      resolve(p)
    })
  })
}

describe('USMDecrypter class', function () {

  it('#demux wrong file', function (done) {
    this.timeout(Infinity)
    fs.mkdirsSync(defaultDir)
    demuxAsync('123', defaultDir).then((p) => {
      done(new Error('should failed'))
    }).catch((err) => {
      console.log(err)
      done()
    })
  })

  it('#demuxSync wrong file', function () {
    this.timeout(Infinity)
    const txtfile = path.join(__dirname, '123.txt')
    fs.writeFileSync(txtfile, 'txt')
    try {
      usm.demuxSync(txtfile, defaultDir)
    } catch (_) {
      console.log(_)
      return
    }
    throw new Error('should failed')
  })

  it('#demux error', async function () {
    this.timeout(Infinity)
    await downloader.downloadOneRaw(ResourceType.MOVIE, '9c59b78e05382f721a8803d9aed06640', usmPath)
    fs.removeSync(defaultDir)
    try {
      await demuxAsync(usmPath)
    } catch (_) {
      return
    }
    throw new Error('should failed')
  })

  it('#demuxSync error', async function () {
    this.timeout(Infinity)

    await downloader.downloadOneRaw(ResourceType.MOVIE, '9c59b78e05382f721a8803d9aed06640', usmPath)
    fs.removeSync(defaultDir)
    try {
      usm.demuxSync(usmPath)
    } catch (_) {
      return
    }
    throw new Error('should failed')
  })

  it('#demux success', async function () {
    this.timeout(Infinity)

    await downloader.downloadOneRaw(ResourceType.MOVIE, '9c59b78e05382f721a8803d9aed06640', usmPath)
    fs.removeSync(defaultDir)
    fs.mkdirsSync(defaultDir)
    const p = await demuxAsync(usmPath)
    assert.ok(p === defaultDir)
    const ls = fs.readdirSync(defaultDir)
    console.log(ls)
    assert.ok(ls.length !== 0)
  })

  it('#demuxSync success', async function () {
    this.timeout(Infinity)

    await downloader.downloadOneRaw(ResourceType.MOVIE, '9c59b78e05382f721a8803d9aed06640', usmPath)
    fs.removeSync(defaultDir)
    fs.mkdirsSync(defaultDir)
    const b = usm.demuxSync(usmPath)
    assert.ok(b === defaultDir)
    const ls = fs.readdirSync(defaultDir)
    assert.ok(ls.length !== 0)
  })

})

