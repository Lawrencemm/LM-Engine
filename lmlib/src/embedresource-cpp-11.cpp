#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <memory>


int main(int argc, char** argv)
{
  if (argc < 3)
  {
    std::cerr << "USAGE: " << argv[0] << " {sym} {rsrc}" << std::endl
              << std::endl
              << "  Creates {sym}.c from the contents of {rsrc}" << std::endl;
    return EXIT_FAILURE;
  }

  // Replace characters in the file path incompatible with a c variable name.
  std::string sym(argv[2]);
  std::replace(sym.begin(), sym.end(), '.', '_');
  std::replace(sym.begin(), sym.end(), '-', '_');
  std::replace(sym.begin(), sym.end(), '/', '_');
  std::replace(sym.begin(), sym.end(), '\\', '_');

  // Get length of input resource.
  std::ifstream ifs;
  ifs.open(argv[2], std::ios_base::ate | std::ios_base::binary);
  if (ifs.fail())
  {
    std::cout << "Failed to open " << argv[2] << '\n';
    return EXIT_FAILURE;
  }
  auto resource_byte_size = ifs.tellg();
  ifs.close();

  // Read input resource into a buffer.
  ifs.open(argv[2], std::ios_base::binary);
  auto buffer = std::unique_ptr<char[]>(new char[resource_byte_size]);
  ifs.read(buffer.get(), resource_byte_size);
  ifs.close();

  // Write output as a .c file.
  std::ofstream ofs;
  ofs.open(argv[1], std::ios_base::binary);
  if (ofs.fail())
  {
    std::cout << "Failed to open " << argv[1] << '\n';
    return EXIT_FAILURE;
  }

  ofs << "#include <stdlib.h>" << std::endl;
  ofs << "extern unsigned char const _resource_" << sym << "[] = {" << std::endl;

  size_t lineCount = 0;
  for (int i = 0; i < resource_byte_size; ++i)
  {
    char c = buffer.get()[i];
    ofs << "0x" << std::hex << (c & 0xff) << ", ";
    if (++lineCount == 10)
    {
      ofs << std::endl;
      lineCount = 0;
    }
  }

  ofs << "};" << std::endl;
  ofs << "extern size_t const _resource_" << sym << "_len = sizeof(_resource_" << sym
      << ");";

  ofs.close();

  return EXIT_SUCCESS;
}
