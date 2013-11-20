/*
 * Copyright 2009 AOL LLC 
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at 
 *         
 *         http://www.apache.org/licenses/LICENSE-2.0 
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

#include "mod_dims.h"

#define MAGICK_CHECK(func, rec) \
    do { \
        apr_status_t code = func; \
        if(rec->status == DIMS_IMAGEMAGICK_TIMEOUT) {\
            return DIMS_IMAGEMAGICK_TIMEOUT; \
        } else if(code == MagickFalse) {\
            return DIMS_FAILURE; \
        } \
    } while(0)

#define MAGICK_CHECK_FREE_ON_FAIL(func, rec, wand) \
    do { \
        apr_status_t code = func; \
        if(rec->status == DIMS_IMAGEMAGICK_TIMEOUT) {\
			DestroyMagickWand(wand); \
            return DIMS_IMAGEMAGICK_TIMEOUT; \
        } else if(code == MagickFalse) {\
			DestroyMagickWand(wand); \
            return DIMS_FAILURE; \
        } \
    } while(0)


/*
apr_status_t
dims_smart_crop_operation (dims_request_rec *d, char *args, char **err) {
    int flags;
    RectangleInfo rec;
    ExceptionInfo ex_info;

    flags = ParseGravityGeometry(MagickGetImage(d->wand), args, &rec, &ex_info);
    if(!(flags & AllValues)) {
        *err = "Parsing crop geometry failed";
        return DIMS_FAILURE;
    }

    // MAGICK_CHECK(MagickResizeImage(d->wand, rec.width, rec.height, UndefinedFilter, 1), d);
    smartCrop(d->wand, 20, rec.width, rec.height);

    return DIMS_SUCCESS;
}
*/

apr_status_t
dims_strip_operation (dims_request_rec *d, char *args, char **err) {

    /* If args is passed from the user and 
     *   a) it equals true, strip the image.
     *   b) it equals false, don't strip the image.
     *   c) it is neither true/false, strip based on config value.
     * If args is NULL, strip based on config value.
     */
    if(args != NULL) {
        if(strcmp(args, "true") == 0 || ( strcmp(args, "false") != 0 && d->config->strip_metadata )) {
            MAGICK_CHECK(MagickStripImage(d->wand), d);
        }
    }
    else if(d->config->strip_metadata) {
        MAGICK_CHECK(MagickStripImage(d->wand), d);
    }

    return DIMS_SUCCESS;
}

apr_status_t
dims_normalize_operation (dims_request_rec *d, char *args, char **err) {
    MAGICK_CHECK(MagickNormalizeImage(d->wand), d);
    return DIMS_SUCCESS;
}

apr_status_t
dims_mirroredfloor_operation (dims_request_rec *d, char *args, char **err) {
	MagickWand *tmp = CloneMagickWand(d->wand);
	if (!tmp) {
		return DIMS_FAILURE;
	}
    MAGICK_CHECK_FREE_ON_FAIL(MagickFlipImage(tmp), d, tmp);
	unsigned long width = MagickGetImageWidth(d->wand);
	unsigned long height = MagickGetImageHeight(d->wand);
	MAGICK_CHECK_FREE_ON_FAIL(MagickExtentImage(d->wand, width, height * 2, 0, 0), d, tmp);
	MAGICK_CHECK_FREE_ON_FAIL(MagickCompositeImage(d->wand, tmp, OverCompositeOp, 0, height), d, tmp);
	DestroyMagickWand(tmp);
	return DIMS_SUCCESS;
}

apr_status_t
dims_flip_operation (dims_request_rec *d, char *args, char **err) {
	MAGICK_CHECK(MagickFlipImage(d->wand), d);
	return DIMS_SUCCESS;
}

apr_status_t
dims_flop_operation (dims_request_rec *d, char *args, char **err) {
	MAGICK_CHECK(MagickFlopImage(d->wand), d);
	return DIMS_SUCCESS;
}

/*
 * Not available in GraphicsMagick
apr_status_t
dims_adaptive_resize_operation (dims_request_rec *d, char *args, char **err) {
    int flags;
    RectangleInfo rec;

	SetGeometry(MagickGetImage(d->wand), &rec);
    flags = GetMagickGeometry(args, &rec.x, &rec.y, &rec.width, &rec.height);
    if(!(flags & AllValues)) {
        *err = "Parsing thumbnail geometry failed";
        return DIMS_FAILURE;
    }

    MAGICK_CHECK(MagickAdaptiveResizeImage(d->wand, rec.width, rec.height), d);

    return DIMS_SUCCESS;
}
*/

/* not available in graphics magick
apr_status_t
dims_liquid_resize_operation (dims_request_rec *d, char *args, char **err) {
    int flags;
    RectangleInfo rec;

	SetGeometry(MagickGetImage(d->wand), &rec);
    flags = GetMagickGeometry(args, &rec.x, &rec.y, &rec.width, &rec.height);
    if(!(flags & AllValues)) {
        *err = "Parsing thumbnail geometry failed";
        return DIMS_FAILURE;
    }

    MAGICK_CHECK(MagickLiquidRescaleImage(d->wand, rec.width, rec.height, 1.0, 0.0), d);

    return DIMS_SUCCESS;
}
*/

apr_status_t
dims_resize_operation (dims_request_rec *d, char *args, char **err) {
    int flags;
    long x = 0, y = 0;
    unsigned long width = MagickGetImageWidth(d->wand);
    unsigned long height = MagickGetImageHeight(d->wand);

    flags = GetMagickGeometry(args, &x, &y, &width, &height);
    if(!(flags & AllValues)) {
        *err = "Parsing thumbnail geometry failed";
        return DIMS_FAILURE;
    }

    MAGICK_CHECK(MagickResizeImage(d->wand, width, height, SincFilter, 0.9), d);

    return DIMS_SUCCESS;
}

apr_status_t
dims_sharpen_operation (dims_request_rec *d, char *args, char **err) {
    int flags;
	double sigma, radius = 0.0;

    flags = GetMagickDimension(args, &radius, &sigma, NULL, NULL);
	if (flags < 1 ) {
		return DIMS_FAILURE;
	}
    if (flags < 2) {
		// sigma can be default
        sigma=1.0;
    }

    MAGICK_CHECK(MagickSharpenImage(d->wand, radius, sigma), d);

    return DIMS_SUCCESS;
}

apr_status_t
dims_thumbnail_operation (dims_request_rec *d, char *args, char **err) {
    int flags;
    char *resize_args = apr_psprintf(d->pool, "%s^", args);
    long x = 0, x2 = 0;
    long y = 0, y2 = 0;
    unsigned long width = MagickGetImageWidth(d->wand);
    unsigned long width2 = width;
    unsigned long height = MagickGetImageHeight(d->wand);
    unsigned long height2 = height;

    flags = GetMagickGeometry(resize_args, &x, &y, &width, &height);
    if(!(flags & AllValues)) {
        *err = "Parsing thumbnail (resize) geometry failed";
        return DIMS_FAILURE;
    }

    MAGICK_CHECK(MagickResizeImage(d->wand, width, height, BoxFilter, 0.9), d);

    if(!(flags & PercentValue)) {
        //flags = ParseAbsoluteGeometry(args, &rec2);
    	flags = GetMagickGeometry(args, &x2, &y2, &width2, &height2);
        if(!(flags & AllValues)) {
            *err = "Parsing thumbnail (crop) geometry failed";
            return DIMS_FAILURE;
        }

        MAGICK_CHECK(MagickCropImage(d->wand, width2, height2, (int)((width - width2) / 2), (int)((height - height2) / 2)), d);
    }
    
    return DIMS_SUCCESS;
}

apr_status_t
dims_crop_operation (dims_request_rec *d, char *args, char **err) {
    int flags;
    long x = 0, y = 0;
    unsigned long width = MagickGetImageWidth(d->wand);
    unsigned long height = MagickGetImageHeight(d->wand);
    ExceptionInfo ex_info;

    flags = GetMagickGeometry(args, &x, &y, &width, &height);
    if(!(flags & AllValues)) {
        *err = "Parsing crop geometry failed";
        return DIMS_FAILURE;
    }

    MAGICK_CHECK(MagickCropImage(d->wand, width, height, x, y), d);

    return DIMS_SUCCESS;
}

apr_status_t
dims_format_operation (dims_request_rec *d, char *args, char **err) {
    MAGICK_CHECK(MagickSetFormat(d->wand, args), d);
    return DIMS_SUCCESS;
}

apr_status_t
dims_quality_operation (dims_request_rec *d, char *args, char **err) {
    int quality = apr_strtoi64(args, NULL, 0);
    MAGICK_CHECK(MagickSetCompressionQuality(d->wand, quality), d);
    return DIMS_SUCCESS;
}

apr_status_t
dims_blur_operation (dims_request_rec *d, char *args, char **err) {
    int flags;
	double radius, sigma;

	flags = GetMagickDimension(args, &radius, &sigma, NULL, NULL);
    if (flags < 1 ) {
		return DIMS_FAILURE;
    }
    if (flags < 2 ) {
        sigma=1.0;
    }

    MAGICK_CHECK(MagickBlurImage(d->wand, radius, sigma), d);

    return DIMS_SUCCESS;
}

dims_brightness_operation (dims_request_rec *d, char *args, char **err) {
    int flags;
	double brightness, contrast;

    flags = GetMagickDimension(args, &brightness, &contrast, NULL, NULL);

	if (flags < 2) {
		return DIMS_FAILURE;
	}

    //MAGICK_CHECK(MagickBrightnessContrastImage(d->wand,
    //        brightness, contrast), d);

    return DIMS_SUCCESS;
}

dims_modulate_operation (dims_request_rec *d, char *args, char **err) {
	// modulate accepts "<brightness>,<saturation>,<hue>"
    double brightness, sat, hue;
    int flags = GetMagickDimension(args, &brightness, &sat, &hue, NULL);
    if (flags < 3) {
		return DIMS_FAILURE;
    }
    MAGICK_CHECK(MagickModulateImage(d->wand, brightness, sat, hue), d);

    return DIMS_SUCCESS;
}

apr_status_t
dims_flipflop_operation (dims_request_rec *d, char *args, char **err) {
    if(args != NULL) {
        if(strcmp(args, "horizontal") == 0) {
            MAGICK_CHECK(MagickFlopImage(d->wand), d);
        } else if (strcmp(args, "vertical") == 0) {
            MAGICK_CHECK(MagickFlipImage(d->wand), d);
        }
    }

    return DIMS_SUCCESS;
}

/*
 * unsupported by GraphicsMagick
apr_status_t
dims_sepia_operation (dims_request_rec *d, char *args, char **err) {
    double threshold = atof(args);

    MAGICK_CHECK(MagickSepiaToneImage(d->wand, threshold * QuantumRange), d);

    return DIMS_SUCCESS;
}
*/

apr_status_t
dims_grayscale_operation (dims_request_rec *d, char *args, char **err) {

    if(args != NULL) {
        if(strcmp(args, "true") == 0) {
            MAGICK_CHECK(MagickSetImageColorspace(d->wand, GRAYColorspace), d);
        }
    }

    return DIMS_SUCCESS;
}

apr_status_t
dims_autolevel_operation (dims_request_rec *d, char *args, char **err) {

    if(args != NULL) {
        if(strcmp(args, "true") == 0) {
            MAGICK_CHECK(MagickEqualizeImage(d->wand), d);
        }
    }

    return DIMS_SUCCESS;
}

apr_status_t
dims_invert_operation (dims_request_rec *d, char *args, char **err) {

    if(args != NULL) {
        if(strcmp(args, "true") == 0) {
            MAGICK_CHECK(MagickNegateImage(d->wand, MagickFalse), d);
        }
    }

    return DIMS_SUCCESS;
}

apr_status_t
dims_rotate_operation (dims_request_rec *d, char *args, char **err) {
    double degrees = atof(args);

    PixelWand *pxWand = NewPixelWand();
    MAGICK_CHECK(MagickRotateImage(d->wand, pxWand, degrees), d);
    DestroyPixelWand(pxWand);

    return DIMS_SUCCESS;
}
