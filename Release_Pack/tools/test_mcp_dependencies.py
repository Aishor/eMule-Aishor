"""Test de verificación de dependencias MCP para eMule-Aishor"""
import sys

def test_dependencies():
    print("========================================")
    print(" Test de Dependencias MCP")
    print("========================================\n")
    
    # Test Python version
    print(f"Python version: {sys.version}")
    if sys.version_info < (3, 11):
        print("⚠️  ADVERTENCIA: Se recomienda Python 3.11+")
    else:
        print("✅ Versión de Python correcta\n")
    
    # Test mcp
    try:
        import mcp
        print(f"✅ mcp instalado (versión: {mcp.__version__ if hasattr(mcp, '__version__') else 'desconocida'})")
    except ImportError:
        print("❌ mcp NO instalado")
        print("   Solución: ejecutar tools\\install_mcp.bat")
        return False
    
    # Test httpx
    try:
        import httpx
        print(f"✅ httpx instalado (versión: {httpx.__version__})")
    except ImportError:
        print("❌ httpx NO instalado")
        print("   Solución: ejecutar tools\\install_mcp.bat")
        return False
    
    # Test asyncio (built-in)
    try:
        import asyncio
        print(f"✅ asyncio disponible")
    except ImportError:
        print("❌ asyncio NO disponible (error crítico)")
        return False
    
    print("\n========================================")
    print(" ✅ TODAS LAS DEPENDENCIAS CONFIRMADAS")
    print("========================================")
    return True

if __name__ == "__main__":
    success = test_dependencies()
    sys.exit(0 if success else 1)
