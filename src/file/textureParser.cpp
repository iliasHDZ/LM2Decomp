#include "textureParser.hpp"

Resource* TextureParser::Parse(ResourceReader* rd, label_node* label) {

}

u32 TextureParser::GetLoadingMethod() {
    return 3;
}

u32 TextureParser::GetMagicNumber() {
    return 0xe977d350;
}