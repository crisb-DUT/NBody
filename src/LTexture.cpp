#include "LTexture.h"



///* Public routines */
//
///*
// * Create an empty RGB surface of the appropriate depth using the given
// * enum SDL_PIXELFORMAT_* format
// */
//SDL_Surface *
//SDL_CreateRGBSurfaceWithFormat(Uint32 flags, int width, int height, int depth,
//                               Uint32 format)
//{
//    SDL_Surface *surface;
//
//    /* The flags are no longer used, make the compiler happy */
//    (void)flags;
//
//    /* Allocate the surface */
//    surface = (SDL_Surface *) SDL_calloc(1, sizeof(*surface));
//    if (surface == NULL) {
//        SDL_OutOfMemory();
//        return NULL;
//    }
//
//    surface->format = SDL_AllocFormat(format);
//    if (!surface->format) {
//        SDL_FreeSurface(surface);
//        return NULL;
//    }
//    surface->w = width;
//    surface->h = height;
//    surface->pitch = SDL_CalculatePitch(surface);
//    SDL_SetClipRect(surface, NULL);
//
//    if (SDL_ISPIXELFORMAT_INDEXED(surface->format->format)) {
//        SDL_Palette *palette =
//                SDL_AllocPalette((1 << surface->format->BitsPerPixel));
//        if (!palette) {
//            SDL_FreeSurface(surface);
//            return NULL;
//        }
//        if (palette->ncolors == 2) {
//            /* Create a black and white bitmap palette */
//            palette->colors[0].r = 0xFF;
//            palette->colors[0].g = 0xFF;
//            palette->colors[0].b = 0xFF;
//            palette->colors[1].r = 0x00;
//            palette->colors[1].g = 0x00;
//            palette->colors[1].b = 0x00;
//        }
//        SDL_SetSurfacePalette(surface, palette);
//        SDL_FreePalette(palette);
//    }
//
//    /* Get the pixels */
//    if (surface->w && surface->h) {
//        /* Assumptions checked in surface_size_assumptions assert above */
//        Sint64 size = ((Sint64)surface->h * surface->pitch);
//        if (size < 0 || size > SDL_MAX_SINT32) {
//            /* Overflow... */
//            SDL_FreeSurface(surface);
//            SDL_OutOfMemory();
//            return NULL;
//        }
//
//        surface->pixels = SDL_malloc((size_t)size);
//        if (!surface->pixels) {
//            SDL_FreeSurface(surface);
//            SDL_OutOfMemory();
//            return NULL;
//        }
//        /* This is important for bitmaps */
//        SDL_memset(surface->pixels, 0, surface->h * surface->pitch);
//    }
//
//    /* Allocate an empty mapping */
//    surface->map = SDL_AllocBlitMap();
//    if (!surface->map) {
//        SDL_FreeSurface(surface);
//        return NULL;
//    }
//
//    /* By default surface with an alpha mask are set up for blending */
//    if (surface->format->Amask) {
//        SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_BLEND);
//    }
//
//    /* The surface is ready to go */
//    surface->refcount = 1;
//    return surface;
//}
//
//


LTexture::LTexture() {
    //Initialize
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;

    renderer = NULL;
    font = NULL;
}

LTexture::~LTexture() {
    //Deallocate
    free();
}

void LTexture::setRenderer(SDL_Renderer *renderer) {
    this->renderer = renderer;
}

void LTexture::setFont(TTF_Font *font) {
    this->font = font;
}

bool LTexture::createFromBuffer(int width, int height, unsigned char *buffer) {
    //Get rid of preexisting texture
    free();

    //The final texture
    SDL_Texture *newTexture = NULL;

    //Load image at specified path
//    SDL_Surface *loadedSurface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 8, SDL_PIXELFORMAT_RGB24);
    SDL_PixelFormat* formatS = SDL_AllocFormat(SDL_PIXELFORMAT_RGB24);

    SDL_Surface *loadedSurface = SDL_CreateRGBSurface(0,width,height,formatS->BitsPerPixel,formatS->Rmask,formatS->Gmask,formatS->Bmask,formatS->Amask);
    //SDL_memset(loadedSurface->pixels, 255, loadedSurface->h * loadedSurface->pitch);
    SDL_memcpy(loadedSurface->pixels, buffer, width * height * 3 * sizeof(unsigned char));

    if (loadedSurface == NULL) {
        printf("Unable to load image! SDL_image Error: %s\n", SDL_GetError());
    } else {
        //Color key image
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

        //Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface(this->renderer, loadedSurface);
        if (newTexture == NULL) {
            printf("Unable to create texture! SDL Error: %s\n", SDL_GetError());
        } else {
            //Get image dimensions
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }

        //Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);
    }

    //Return success
    mTexture = newTexture;
    return mTexture != NULL;
}

bool LTexture::loadFromFile(std::string path) {
    //Get rid of preexisting texture
    free();

    //The final texture
    SDL_Texture *newTexture = NULL;

    //Load image at specified path
    SDL_Surface *loadedSurface = SDL_LoadBMP(path.c_str());
    if (loadedSurface == NULL) {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), SDL_GetError());
    } else {
        //Color key image
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

        //Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface(this->renderer, loadedSurface);
        if (newTexture == NULL) {
            printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
        } else {
            //Get image dimensions
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }

        //Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);
    }

    //Return success
    mTexture = newTexture;
    return mTexture != NULL;
}

bool LTexture::loadFromMemory(unsigned char *buffer) {
    //Get rid of preexisting texture
    free();

    //The final texture
    SDL_Texture *newTexture = NULL;

    //Load image at specified path
    SDL_RWops *RWOP = SDL_RWFromMem(buffer, sizeof(buffer));
    SDL_Surface *loadedSurface = SDL_LoadBMP_RW(RWOP, 1);

    if (loadedSurface == NULL) {
        printf("Unable to load image! SDL_image Error: %s\n", SDL_GetError());
    } else {
        //Color key image
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

        //Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface(this->renderer, loadedSurface);
        if (newTexture == NULL) {
            printf("Unable to create texture! SDL Error: %s\n", SDL_GetError());
        } else {
            //Get image dimensions
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }

        //Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);
    }

    //Return success
    mTexture = newTexture;
    return mTexture != NULL;
}

bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor) {
    //Get rid of preexisting texture
    free();

    //Render text surface
    SDL_Surface *textSurface = TTF_RenderText_Solid(this->font, textureText.c_str(), textColor);
    if (textSurface == NULL) {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
    } else {
        //Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface(this->renderer, textSurface);
        if (mTexture == NULL) {
            printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
        } else {
            //Get image dimensions
            mWidth = textSurface->w;
            mHeight = textSurface->h;
        }

        //Get rid of old surface
        SDL_FreeSurface(textSurface);
    }

    //Return success
    return mTexture != NULL;
}

void LTexture::free() {
    //Free texture if it exists
    if (mTexture != NULL) {
        SDL_DestroyTexture(mTexture);
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
    }
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue) {
    //Modulate texture rgb
    SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending) {
    //Set blending function
    SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha) {
    //Modulate texture alpha
    SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render(int x, int y, SDL_Rect *clip, double angle, SDL_Point *center, SDL_RendererFlip flip) {
    //Set rendering space and render to screen
    SDL_Rect renderQuad = {x, y, mWidth, mHeight};

    //Set clip rendering dimensions
    if (clip != NULL) {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    //Render to screen
    SDL_RenderCopyEx(this->renderer, mTexture, clip, &renderQuad, angle, center, flip);
}

int LTexture::getWidth() {
    return mWidth;
}

int LTexture::getHeight() {
    return mHeight;
}