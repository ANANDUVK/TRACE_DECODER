def print_hex_from_file(filename, size):
    try:
        with open(filename, "rb") as f:
            data = f.read(size)

        # Print hex output grouped in rows of 16 bytes
        for i in range(0, len(data), 16):
            chunk = data[i:i+16]
            hex_values = ' '.join(f"{byte:02X}" for byte in chunk)
            #print(f"{i:08X}: {hex_values}")
            print(f"{hex_values}")

    except FileNotFoundError:
        print(f"Error: File '{filename}' not found.")
    except Exception as e:
        print(f"Error reading file: {e}")


# Example usage:
if __name__ == "__main__":
    filename = "ntracedata.rtd"   # change to your file name
    size_to_read = 1024       # change to desired byte count
    print_hex_from_file(filename, size_to_read)
