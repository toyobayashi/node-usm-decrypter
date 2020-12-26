const { USMDecrypter } = require('..')
const { Downloader, ResourceType } = require('mishiro-core')
const fs = require('fs-extra')
const path = require('path')
const assert = require('assert')

const getPath = (...args) => {
  return path.join(__dirname, '..', ...args)
}

const downloader = new Downloader()
const usm = new USMDecrypter()

const usmPath = getPath('./test/movie8028.usm')
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

  it('#demux error', async function () {
    this.timeout(Infinity)
    await downloader.downloadOneRaw(ResourceType.MOVIE, 'a716a94be7d7c51a641971ca4fc79ff5', usmPath)
    fs.removeSync(defaultDir)
    try {
      await demuxAsync(usmPath)
      throw new Error('should failed')
    } catch (_) {}
  })

  it('#demuxSync error', async function () {
    this.timeout(Infinity)

    await downloader.downloadOneRaw(ResourceType.MOVIE, 'a716a94be7d7c51a641971ca4fc79ff5', usmPath)
    fs.removeSync(defaultDir)
    try {
      usm.demuxSync(usmPath)
      done(new Error('should failed'))
    } catch (_) {}
  })

  it('#demux success', async function () {
    this.timeout(Infinity)

    await downloader.downloadOneRaw(ResourceType.MOVIE, 'a716a94be7d7c51a641971ca4fc79ff5', usmPath)
    fs.removeSync(defaultDir)
    fs.mkdirsSync(defaultDir)
    const p = await demuxAsync(usmPath)
    assert.ok(p === defaultDir)
  })

  it('#demuxSync success', async function () {
    this.timeout(Infinity)

    await downloader.downloadOneRaw(ResourceType.MOVIE, 'a716a94be7d7c51a641971ca4fc79ff5', usmPath)
    fs.removeSync(defaultDir)
    fs.mkdirsSync(defaultDir)
    const b = usm.demuxSync(usmPath)
    assert.ok(b)
  })

})

