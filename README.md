```ts
export declare type DecryptCallback = (err: Error | null, outdir: string) => void

export declare enum Encoding {
  UTF8 = 65001,
  SHIFT_JIS = 932
}

export declare class USMDecrypter {
  constructor(cipherKey1?: number, cipherKey2?: number)

  getEncoding (): Encoding
  setEncoding (encoding: Encoding): void

  demux (usmFile: string, callback?: DecryptCallback): void
  demux (usmFile: string, outdir: string, callback?: DecryptCallback): void
  demux (usmFile: string, outdir: string, adxDecode: boolean, callback?: DecryptCallback): void

  demuxSync (usmFile: string, outdir?: string, adxDecode?: boolean): string
}
```
