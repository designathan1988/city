import sys, numpy as np
from PIL import Image
import colorsys
def load(p): return np.asarray(Image.open(p).convert('RGB')).astype(np.float64)/255.0
def hsv(a):
    mx=a.max(-1); mn=a.min(-1); d=mx-mn
    s=np.where(mx>0,d/np.maximum(mx,1e-9),0)
    return s, mx
def lin(c): return np.where(c<=0.04045,c/12.92,((c+0.055)/1.055)**2.4)
def lum(a):  # Rec.709 luma on gamma values
    return 0.2126*a[...,0]+0.7152*a[...,1]+0.0722*a[...,2]
def stats(name,a,box):
    x0,y0,x1,y1=box; r=a[y0:y1,x0:x1].reshape(-1,3)
    s,v=hsv(r); L=lum(r)
    m=(r.mean(0)*255).round().astype(int)
    p=np.percentile(L*255,[5,50,95]).round().astype(int)
    print(f"  {name:22s} RGB medio {tuple(m)} | lum p5/p50/p95 {tuple(p)} | desvio lum {L.std()*255:5.1f} | sat media {s.mean():.2f} | B/R {r[:,2].mean()/max(r[:,0].mean(),1e-9):.2f}")
imgs={'4.0 (87-demolish)':'reference/87-demolish.png','jogo 13:55':'evidence/shell/shell_drawer_demolish.png','jogo 14:34':'evidence/features/shell_drawer_demolish.png'}
boxes={'canto sup. esq. (terreno)':(0,70,300,250),'canto sup. dir. (terreno)':(1300,70,1600,250),'cidade centro':(600,250,1000,520),'cidade esq.':(250,450,550,700),'cidade dir.':(1100,350,1400,600)}
for n,p in imgs.items():
    a=load(p); print(n)
    for bn,b in boxes.items(): stats(bn,a,b)
    # gradiente medio (detalhe) no centro
    L=lum(a[250:520,600:1000]); gx=np.abs(np.diff(L,axis=1)).mean()*255; gy=np.abs(np.diff(L,axis=0)).mean()*255
    print(f"  detalhe (gradiente medio no centro): {(gx+gy)/2:.2f}")
