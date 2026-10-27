# Digital Image Steganography using LSB in C

[![Language](https://img.shields.io/badge/Language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Standard](https://img.shields.io/badge/Standard-C99-yellow.svg)](https://en.wikipedia.org/wiki/C99)
[![Format](https://img.shields.io/badge/Format-24--bit%20BMP-orange.svg)](https://en.wikipedia.org/wiki/BMP_file_format)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

A robust, register- and bit-level implementation of **Digital Image Steganography** in C. The application encodes secret arbitrary text/files into the **Least Significant Bit (LSB)** of 24-bit uncompressed BMP image pixel bytes without causing perceptible visual degradation, and accurately decodes the payload back to its original form.

---

## 📌 How It Works

### Encoding Workflow
1. **Header Preservation**: The original 54-byte BMP header is copied directly to maintain valid bitmap format properties.
2. **Magic String Embedding**: Encodes `#*` into the image to verify authentic stego images during decoding.
3. **Secret File Extension & Size**: Encodes the secret file extension length, extension string (e.g., `.txt`), and file size (in bytes).
4. **Data Bit Insertion**: Iterates through each byte of the secret payload. Each bit is embedded into the LSB of one byte of image pixel data (8 image bytes per 1 secret byte).
5. **Remaining Bytes**: Copies the remaining unencoded image pixel data to output image.

```text
Secret Bit:               1
Image Byte:       1 0 1 1 0 1 0 [0]  <- Original LSB
Stego Image Byte: 1 0 1 1 0 1 0 [1]  <- Embedded LSB
```

### Decoding Workflow
1. **Magic String Verification**: Reads the first encoded bits and verifies against `#*`. Rejects invalid images.
2. **Metadata Extraction**: Decodes the extension length, extension characters, and secret file size.
3. **Payload Reconstruction**: Reads sequential image bytes, reconstructs secret bytes bit-by-bit from the LSBs, and writes to the destination file.

---

## 🏗️ Project Structure

| File | Description |
| :--- | :--- |
| [`test_encode.c`](test_encode.c) | Entry point (`main`) & CLI argument dispatcher |
| [`encode.c`](encode.c) / [`encode.h`](encode.h) | Encoding implementation & capacity validation |
| [`decode.c`](decode.c) / [`decode.h`](decode.h) | Decoding implementation & file reconstruction |
| [`common.h`](common.h) | Global definitions (`MAGIC_STRING "#*"`) |
| [`types.h`](types.h) | Custom enum return types (`Status`, `OperationType`) |
| [`Makefile`](Makefile) | Compilation and build automation script |

---

## ⚙️ Build and Installation

### Prerequisites
- GCC / Clang compiler
- Make (optional)

### Using Makefile
```bash
# Build the executable
make

# Clean build artifacts
make clean
```

### Manual Compilation
```bash
gcc -Wall -Wextra test_encode.c encode.c decode.c -o stego
```

---

## 💻 Usage

### 1. Encoding (Hiding secret text into an image)
```bash
./stego -e beautiful.bmp secret.txt [output.bmp]
```
* Arguments:
  * `-e`: Encoding mode
  * `beautiful.bmp`: Source 24-bit BMP image
  * `secret.txt`: Text file containing the secret message
  * `output.bmp`: *(Optional)* Name for stego-output image (defaults to `stego.bmp`)

### 2. Decoding (Extracting secret text from stego image)
```bash
./stego -d encoded.bmp [output.txt]
```
* Arguments:
  * `-d`: Decoding mode
  * `encoded.bmp`: Stego BMP image containing hidden payload
  * `output.txt`: *(Optional)* Output text file name (defaults to `decode.txt`)

---

## 🔬 Sample Terminal Execution

```text
$ ./stego -e beautiful.bmp secret.txt encoded.bmp
## Selected encoding ##
INFO: Opening required files
INFO: Done
INFO: Checking image capacity
INFO: Done. Found OK
INFO: Copying Image Header
INFO: Done
INFO: Encoding Magic String Signature
INFO: Done
INFO: Encoding secret.txt File Extension Size
INFO: Done
INFO: Encoding secret.txt File Extension
INFO: Done
INFO: Encoding secret.txt File Size
INFO: Done
INFO: Encoding secret.txt File Data
INFO: Done
INFO: Copying Left Over Data
INFO: Done
## Encoding Done Successfully ##
```

---

## 👤 Author

**Nilesh Mahajan**  
- GitHub: [@Nileshkm911](https://github.com/Nileshkm911)
- LinkedIn: [Nilesh Mahajan](https://linkedin.com/in/nilesh-mahajan-150a1b2a5)
