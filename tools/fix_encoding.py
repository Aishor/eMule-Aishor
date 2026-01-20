import os

def fix_file(file_path):
    if not os.path.exists(file_path):
        print(f"[SKIP] Not found: {file_path}")
        return

    try:
        with open(file_path, 'rb') as f:
            content = f.read()
        
        # Check for UTF-16 LE BOM (FF FE)
        if content.startswith(b'\xff\xfe'):
            print(f"[FIXING] UTF-16 BOM detected in: {file_path}")
            decoded = content.decode('utf-16')
            with open(file_path, 'w', encoding='utf-8') as f:
                f.write(decoded)
            print("[OK] Converted to UTF-8")
        else:
            print(f"[OK] No UTF-16 BOM detected in: {file_path}")

    except Exception as e:
        print(f"[ERROR] processing {file_path}: {e}")

if __name__ == '__main__':
    print("--- Fixing Encodings ---")
    fix_file('emule_mcp_server.py')
    fix_file('../Release_Pack/tools/emule_mcp_server.py')
    print("--- Done ---")
