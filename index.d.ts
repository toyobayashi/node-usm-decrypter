export declare type decodeCallback = (err: Error | null, outdir?: string) => void

export declare class USMDecrypter {
  constructor(cipherKey1?: number, cipherKey2?: number)

  demux (usmFile: string, callback?: decodeCallback): void
  demux (usmFile: string, outdir: string, callback?: decodeCallback): void
  demux (usmFile: string, outdir: string, adxDecode: boolean, callback?: decodeCallback): void

  demuxSync (usmFile: string, outdir?: string, adxDecode?: boolean): boolean
}
