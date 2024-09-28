const fs = require('fs');
const zlib = require('zlib');

function parseMCA(filename) {
    const buffer = fs.readFileSync(filename);

    const sectors = [];
    const timestamps = [];

    // ロケーションテーブルを読み取る
    for (let i = 0; i < 1024; i++) {
        const offset = buffer.readUIntBE(i * 4, 3);
        const sectorCount = buffer.readUInt8(i * 4 + 3);
        sectors.push({ offset, sectorCount });
    }

    // タイムスタンプテーブルを読み取る
    for (let i = 0; i < 1024; i++) {
        const timestamp = buffer.readUInt32BE(4096 + i * 4);
        timestamps.push(timestamp);
    }

    const chunks = [];

    for (let i = 0; i < 1024; i++) {
        const sector = sectors[i];
        if (sector.offset === 0 || sector.sectorCount === 0) {
            continue; // 空のチャンク
        }

        const chunkOffset = sector.offset * 4096;
        const length = buffer.readUInt32BE(chunkOffset);
        const chunkData = buffer.slice(chunkOffset + 4, chunkOffset + 4 + length);
        const compressionType = chunkData.readUInt8(0);
        const compressedData = chunkData.slice(1);

        let data;
        if (compressionType === 2) { // zlib圧縮
            data = zlib.inflateSync(compressedData);
        } else if (compressionType === 1) { // GZip圧縮
            data = zlib.unzipSync(compressedData);
        } else {
            console.error(`未知の圧縮タイプ ${compressionType} at chunk ${i}`);
            continue;
        }

        // NBTデータをパースする
        const nbtData = parseNBT(data);

        chunks.push({
            index: i,
            xPos: i % 32,
            zPos: Math.floor(i / 32),
            timestamp: timestamps[i],
            data: nbtData
        });
    }

    return chunks;
}

function parseNBT(buffer) {
    let offset = 0;

    function readTag() {
        if (offset >= buffer.length) {
            return null;
        }

        const tagType = buffer.readUInt8(offset++);
        if (tagType === 0) { // TAG_End
            return null;
        }

        // タグの名前を読み取る
        const nameLength = buffer.readUInt16BE(offset);
        offset += 2;
        const name = buffer.toString('utf8', offset, offset + nameLength);
        offset += nameLength;

        const value = readPayload(tagType);

        return {
            type: getTagTypeName(tagType),
            name: name,
            value: value
        };
    }

    function readPayload(tagType) {
        switch (tagType) {
            case 1: // TAG_Byte
                return buffer.readInt8(offset++);
            case 2: // TAG_Short
                const short = buffer.readInt16BE(offset);
                offset += 2;
                return short;
            case 3: // TAG_Int
                const int = buffer.readInt32BE(offset);
                offset += 4;
                return int;
            case 4: // TAG_Long
                const high = buffer.readInt32BE(offset);
                const low = buffer.readUInt32BE(offset + 4);
                offset += 8;
                return BigInt(high) << 32n | BigInt(low);
            case 5: // TAG_Float
                const float = buffer.readFloatBE(offset);
                offset += 4;
                return float;
            case 6: // TAG_Double
                const double = buffer.readDoubleBE(offset);
                offset += 8;
                return double;
            case 7: // TAG_Byte_Array
                const length = buffer.readInt32BE(offset);
                offset += 4;
                const byteArray = buffer.slice(offset, offset + length);
                offset += length;
                return Array.from(byteArray);
            case 8: // TAG_String
                const strLength = buffer.readUInt16BE(offset);
                offset += 2;
                const str = buffer.toString('utf8', offset, offset + strLength);
                offset += strLength;
                return str;
            case 9: // TAG_List
                const itemType = buffer.readUInt8(offset++);
                const listLength = buffer.readInt32BE(offset);
                offset += 4;
                const list = [];
                for (let i = 0; i < listLength; i++) {
                    list.push(readPayload(itemType));
                }
                return list;
            case 10: // TAG_Compound
                const compound = {};
                while (true) {
                    const tag = readTag();
                    if (tag === null) { // TAG_End
                        break;
                    }
                    compound[tag.name] = tag.value;
                }
                return compound;
            case 11: // TAG_Int_Array
                const intArrayLength = buffer.readInt32BE(offset);
                offset += 4;
                const intArray = [];
                for (let i = 0; i < intArrayLength; i++) {
                    intArray.push(buffer.readInt32BE(offset));
                    offset += 4;
                }
                return intArray;
            case 12: // TAG_Long_Array
                const longArrayLength = buffer.readInt32BE(offset);
                offset += 4;
                const longArray = [];
                for (let i = 0; i < longArrayLength; i++) {
                    const high = buffer.readInt32BE(offset);
                    const low = buffer.readUInt32BE(offset + 4);
                    offset += 8;
                    longArray.push(BigInt(high) << 32n | BigInt(low));
                }
                return longArray;
            default:
                throw new Error(`未知のタグタイプ: ${tagType}`);
        }
    }

    function getTagTypeName(tagType) {
        switch (tagType) {
            case 0: return 'TAG_End';
            case 1: return 'TAG_Byte';
            case 2: return 'TAG_Short';
            case 3: return 'TAG_Int';
            case 4: return 'TAG_Long';
            case 5: return 'TAG_Float';
            case 6: return 'TAG_Double';
            case 7: return 'TAG_Byte_Array';
            case 8: return 'TAG_String';
            case 9: return 'TAG_List';
            case 10: return 'TAG_Compound';
            case 11: return 'TAG_Int_Array';
            case 12: return 'TAG_Long_Array';
            default: return 'Unknown';
        }
    }

    const rootTag = readTag();

    return rootTag;
}

function jsonStringifyWithBigInt(obj) {
    return JSON.stringify(obj, (key, value) =>
        typeof value === 'bigint' ? value.toString() : value
    , 2);
}

// 使用例
const filename = process.argv[2]; // 'r.0.0.mca'; // 解析したいMCAファイルのパスを指定
if(!filename) throw "need_input_file";

const chunks = parseMCA(filename);

// JSONに変換
const json = jsonStringifyWithBigInt(chunks);

// ファイルに保存
fs.writeFileSync(filename+'.json', json);

console.log('MCAファイルを解析し、output.jsonに保存しました。');
