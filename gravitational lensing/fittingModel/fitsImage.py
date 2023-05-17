import matplotlib.pyplot as plt
import numpy as np
from astropy.visualization import simple_norm
from astropy.visualization import astropy_mpl_style
from astropy.utils.data import get_pkg_data_filename
from astropy.io import fits
from astropy import wcs
plt.style.use(astropy_mpl_style)


def crop_image(name, RA=0.0, DEC=0.0, sizex=124, sizey=124):
    image_file = get_pkg_data_filename(name)
    # fits.info(image_file)
    image_data = fits.getdata(image_file)
    hdul = fits.open(name)
    hdr = hdul[1].header
    w = wcs.WCS(hdul[1].header)
    if not RA:
        RA = hdr["RA_APER"]
    if not DEC:
        DEC = hdr["DEC_APER"]

    px, py = w.wcs_world2pix(RA, DEC, 0)

    px = np.round(px).astype(int)
    py = np.round(py).astype(int)
    return image_data[py-sizey//2+3:py+sizey//2-1, px-sizex//2+2:px+sizex//2-2]

def cut_images():
    image_file = get_pkg_data_filename("resources/images/hst_mos_0034519_acs_wfc_f814w_drz.fits")
    image_data = fits.getdata(image_file)
    hdul = fits.open("resources/images/hst_mos_0034519_acs_wfc_f814w_drz.fits")
    hdr = hdul[1].header
    w = wcs.WCS(hdul[1].header)

    # RA = hdr["RA_APER"]
    # DEC = hdr["DEC_APER"]

    RA = 7.282410727099833
    DEC = -0.9307057957904298

    size = 124
    cropped = crop_image("resources/images/hst_mos_0034519_acs_wfc_f814w_drz.fits", RA, DEC, size, size)
    shape = cropped.shape
    lens = cropped
    clearlens = lens
    # lens = plt.imread("output_images/lens1.png")
    flatten = cropped.flatten()
    minv, maxv = 0, 1
    plt.imshow(lens, cmap='viridis', norm=simple_norm(image_data, 'linear', min_cut=minv, max_cut=maxv), origin='upper')
    # plt.grid(False)
    # plt.hist(flatten[flatten < 1.9], bins=200)

    plt.imsave('output_images/lens.png', lens, cmap='viridis', vmin=0, vmax=1)
    # plt.show()
    for i in range(shape[0]):
        for j in range(shape[1]):
            if (i - shape[0]/2)**2 + (j - shape[1]/2)**2 < (shape[0]/6.2)**2 and i > shape[0] / 2:
                lens[i][j] = 0
            elif (i - shape[0]/2)**2 + (j - shape[1]/2)**2 < (shape[0]/5.0)**2 and i <= shape[0] / 2:
                lens[i][j] = 0
    # lens[lens > 0.3] = 0
    # lens[lens < 0.24] = 0
    # lens[lens != 0] = 1

    # print(type(lens[0][0]))
    for i in range(shape[0]):
        for j in range(shape[1]):
            if (i <= shape[0] / 2 and lens[i][j] < 0.24) or (i > shape[0] / 2 and lens[i][j] < 0.295):
                lens[i][j] = 0
            else:
                lens[i][j] = 1
    # print(lens)
    flatten = cropped.flatten()
    minv, maxv = 0, 1
    # lens *= clearlens
    # plt.imshow(lens, cmap='viridis', norm=simple_norm(image_data, 'linear', min_cut=minv, max_cut=maxv), origin='lower')
    # plt.grid(False)
    # plt.hist(flatten[flatten < 1.9], bins=200)

    # plt.grid(False)
    plt.show()
    plt.imsave("output_images/mask.png", lens)

    return lens

print(cut_images())
