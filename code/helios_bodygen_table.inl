
#ifndef BODY
#define BODY(name,type,isRoot,...) {name,type,isRoot, __VA_ARGS__},
#endif

#ifndef STAR
#define STAR(name, ...) BODY(name, Star, true, __VA_ARGS__)
#endif

#ifndef PLANET
#define PLANET(name, ...) BODY(name, Planet, false, __VA_ARGS__)
#endif

#ifndef PLANET
#define PLANET(name, ...) BODY(name, Moon, false, __VA_ARGS__)
#endif

#ifndef MOON
#define MOON(name, ...) BODY(name, Moon, false, __VA_ARGS__)
#endif

#ifndef RGB
#define RGB(r,g,b) {1.0, r, g, b, FMT_ARGB}
#endif

#define MASS(m) m
#define RADIUS(r) r
#define DIS(d) d
#define PERIDEG(deg) deg
#define ECC(e) e

STAR(Sun, RGB(1.000f, 0.843f, 0.000f), MASS(1.9885e30), RADIUS(6.957e8), DIS(0.0), PERIDEG(0.0), ECC(0.0))
	PLANET(Mercury, RGB(0.686f, 0.694f, 0.702f), MASS(3.3011e23), RADIUS(2439700.0), DIS(5.7909e10),  PERIDEG(29.124),  ECC(0.2056))


	PLANET(Venus,   RGB(0.898f, 0.812f, 0.682f), MASS(4.8675e24), RADIUS(6051800.0), DIS(1.0821e11),  PERIDEG(54.884),  ECC(0.0068))

	PLANET(Earth,   RGB(0.251f, 0.408f, 0.686f), MASS(5.9722e24), RADIUS(6371000.0), DIS(1.4960e11),  PERIDEG(114.208), ECC(0.0167))
		MOON(Luna,RGB(0.700f, 0.700f, 0.700f), MASS(7.342e22),  RADIUS(1737400.0),  DIS(3.844e8),   PERIDEG(0.0),     ECC(0.0549))

	PLANET(Mars,    RGB(0.784f, 0.380f, 0.259f), MASS(6.4171e23), RADIUS(3389500.0), DIS(2.2794e11),  PERIDEG(286.502), ECC(0.0934))

	PLANET(Jupiter, RGB(0.784f, 0.694f, 0.584f), MASS(1.8982e27), RADIUS(69911000.0), DIS(7.7834e11), PERIDEG(273.867), ECC(0.0484))

	PLANET(Saturn,  RGB(0.878f, 0.804f, 0.639f), MASS(5.6834e26), RADIUS(58232000.0), DIS(1.4267e12), PERIDEG(339.392), ECC(0.0541))

	PLANET(Uranus,  RGB(0.647f, 0.812f, 0.851f), MASS(8.6810e25), RADIUS(25362000.0), DIS(2.8707e12), PERIDEG(96.999),  ECC(0.0472))

	PLANET(Neptune, RGB(0.247f, 0.435f, 0.875f), MASS(1.0243e26), RADIUS(24622000.0), DIS(4.4984e12), PERIDEG(276.340), ECC(0.0086))

#undef MASS
#undef RADIUS
#undef DIS
#undef PERIDEG
#undef ECC
#undef RGB
#undef PLANET
#undef STAR
#undef BODY
