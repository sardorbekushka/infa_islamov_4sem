import cv2
import numpy as np
import matplotlib.pyplot as plt
from pathlib import Path
from matplotlib import rc

rc('font', **{'family': 'Times new roman'})
rc('text', usetex=True)
# rc('text.latex',unicode=True)
rc('text.latex', preamble=r'\usepackage[utf8]{inputenc}')
rc('text.latex', preamble=r'\usepackage[russian]{babel}')


M_sun = 1.98847e30

#read the mask image
img0 = cv2.imread('output_images/mask.png', 2)
img = cv2.resize(img0, (500,500), interpolation=cv2.INTER_CUBIC)
#cv2.imshow("Binary", img)
#cv2.waitKey(0)
#cv2.destroyAllWindows()
ret, mask = cv2.threshold(img, 127, 255, cv2.THRESH_BINARY)


path=Path("output_images/fitImages")

#read generated images
masses= []
errs = []
for imagepath in path.glob("*.png"):
    i = cv2.imread(str(imagepath), 2)
    ret1, im = cv2.threshold(i, 127, 255, cv2.THRESH_BINARY)
    # print(im.shape)
    err = (im - mask) // 255
    abs_err = np.abs(err)
    #plt.imshow(abs_err)
    #plt.show()
    mae = np.sum(abs_err)
    #print(mae)
    name = str(imagepath)[:-4]
    mass = float(name.partition('mass_')[2])
    errs.append(mae)
    masses.append(mass)


order = np.argsort(masses)
xs = np.array(masses)[order]/(10**11)/M_sun
ys = 100 - np.array(errs)[order]/2500

arg = np.argmax(ys)
diff = 0.9998*ys[arg] # difference is 0.02%
for i in range(arg):
    if (ys[i] >= diff) and (ys[i-1] <= diff):
        x1 = (diff-ys[i-1])/(ys[i]-ys[i-1])*(xs[i]-xs[i-1])+xs[i-1]
for i in range(arg, xs.size):
    if (ys[i] <= diff) and (ys[i-1] >= diff):
        x2 = (diff-ys[i-1])/(ys[i]-ys[i-1])*(xs[i]-xs[i-1])+xs[i-1]

dr = 0.015 * xs[arg]
d1 = xs[arg]-x1 + dr
d2 = x2-xs[arg] + dr


fig, ax = plt.subplots()
ax.plot(xs,ys)
ax.errorbar(xs[arg], ys[arg]+0.003, xerr=[[d1],[d2]], fmt='ro', ecolor = 'red', capsize= 3)
norm = ax.get_ybound()[1]-ax.get_ybound()[0]
ax.axvline(x = xs[arg], ymax = (ys[arg]-ax.get_ybound()[0])/norm, color = 'red',  linestyle = '--')
ax.axvline(x = xs[arg] - d1, ymax = (99.887-ax.get_ybound()[0])/norm, color = 'red',  linestyle = '--')
# ax.axvline(x = xs[arg] - d1, ymax = (diff-ax.get_ybound()[0])/norm, color = 'red',  linestyle = '--')
ax.axvline(x = xs[arg] + d2, ymax = (99.895-ax.get_ybound()[0])/norm, color = 'red',  linestyle = '--')
# ax.axvline(x = xs[arg] + d2, ymax = (diff-ax.get_ybound()[0])/norm, color = 'red',  linestyle = '--')
ax.set_xlabel(xlabel=r'Mass, $10^{11}M_{\odot}$', math_fontfamily='cm')
ax.set_ylabel(ylabel=r'Match, $\%$', math_fontfamily='cm')



textstr = r'$M=%.2f^{+%.2f}_{-%.2f} \cdot 10^{11}\cdot M_{\odot}$' % (xs[arg], d2, d1)
props = dict(boxstyle='round', facecolor='white', alpha=0.5)
ax.text(0.55, 0.17, textstr, transform=ax.transAxes, fontsize=14,
        verticalalignment='top', bbox=props, math_fontfamily='cm')
ax.set_title('Searching for the best fit')
#
# for tick in ax.get_xticklabels():
#     tick.set_fontname("Times New Roman")
# for tick in ax.get_yticklabels():
#     tick.set_fontname("Times New Roman")
ax.grid()

plt.show()











