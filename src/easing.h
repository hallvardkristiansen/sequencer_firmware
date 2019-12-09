#ifndef PI
#define PI 3.1415926545
#endif

double easeInSine( double t ) {
	return sin( 1.5707963 * t );
}

double easeOutSine( double t ) {
	return 1 + sin( 1.5707963 * (--t) );
}

double easeInOutSine( double t ) {
	return 0.5 * (1 + sin( 3.1415926 * (t - 0.5) ) );
}

double easeInQuad( double t ) {
    return t * t;
}

double easeOutQuad( double t ) {
    return t * (2 - t);
}

double easeInOutQuad( double t ) {
    return t < 0.5 ? 2 * t * t : t * (4 - 2 * t) - 1;
}

double easeInCubic( double t ) {
    return t * t * t;
}

double easeOutCubic( double t ) {
    return 1 + (--t) * t * t;
}

double easeInOutCubic( double t ) {
    return t < 0.5 ? 4 * t * t * t : 1 + (--t) * (2 * (--t)) * (2 * t);
}

double easeInQuart( double t ) {
    t *= t;
    return t * t;
}

double easeOutQuart( double t ) {
    t = (--t) * t;
    return 1 - t * t;
}

double easeInOutQuart( double t ) {
    if( t < 0.5 ) {
        t *= t;
        return 8 * t * t;
    } else {
        t = (--t) * t;
        return 1 - 8 * t * t;
    }
}

double easeInQuint( double t ) {
    double t2 = t * t;
    return t * t2 * t2;
}

double easeOutQuint( double t ) {
    double t2 = (--t) * t;
    return 1 + t * t2 * t2;
}

double easeInOutQuint( double t ) {
    double t2;
    if( t < 0.5 ) {
        t2 = t * t;
        return 16 * t * t2 * t2;
    } else {
        t2 = (--t) * t;
        return 1 + 16 * t * t2 * t2;
    }
}

double easeInExpo( double t ) {
    return (pow( 2, 8 * t ) - 1) / 255;
}

double easeOutExpo( double t ) {
    return 1 - pow( 2, -8 * t );
}

double easeInOutExpo( double t ) {
    if( t < 0.5 ) {
        return (pow( 2, 16 * t ) - 1) / 510;
    } else {
        return 1 - 0.5 * pow( 2, -16 * (t - 0.5) );
    }
}

double easeInCirc( double t ) {
    return 1 - sqrt( 1 - t );
}

double easeOutCirc( double t ) {
    return sqrt( t );
}

double easeInOutCirc( double t ) {
    if( t < 0.5 ) {
        return (1 - sqrt( 1 - 2 * t )) * 0.5;
    } else {
        return (1 + sqrt( 2 * t - 1 )) * 0.5;
    }
}

double easeInBack( double t ) {
    return t * t * (2.70158 * t - 1.70158);
}

double easeOutBack( double t ) {
    return 1 + (--t) * t * (2.70158 * t + 1.70158);
}

double easeInOutBack( double t ) {
    if( t < 0.5 ) {
        return t * t * (7 * t - 2.5) * 2;
    } else {
        return 1 + (--t) * t * 2 * (7 * t + 2.5);
    }
}

double easeInElastic( double t ) {
    double t2 = t * t;
    return t2 * t2 * sin( t * PI * 4.5 );
}

double easeOutElastic( double t ) {
    double t2 = (t - 1) * (t - 1);
    return 1 - t2 * t2 * cos( t * PI * 4.5 );
}

double easeInOutElastic( double t ) {
    double t2;
    if( t < 0.45 ) {
        t2 = t * t;
        return 8 * t2 * t2 * sin( t * PI * 9 );
    } else if( t < 0.55 ) {
        return 0.5 + 0.75 * sin( t * PI * 4 );
    } else {
        t2 = (t - 1) * (t - 1);
        return 1 - 8 * t2 * t2 * sin( t * PI * 9 );
    }
}

double easeInBounce( double t ) {
    return pow( 2, 6 * (t - 1) ) * abs( sin( t * PI * 3.5 ) );
}

double easeOutBounce( double t ) {
    return 1 - pow( 2, -6 * t ) * abs( cos( t * PI * 3.5 ) );
}

double easeInOutBounce( double t ) {
    if( t < 0.5 ) {
        return 8 * pow( 2, 8 * (t - 1) ) * abs( sin( t * PI * 7 ) );
    } else {
        return 1 - 8 * pow( 2, -8 * t ) * abs( sin( t * PI * 7 ) );
    }
}

double get_easing(double time) {
	double mult = 1.0;
	switch(glide_mode) {
		case 0:
			mult = easeInOutSine(time);
		break;
		case 1:
			mult = easeInSine(time);
		break;
		case 2:
			mult = easeOutSine(time);
		break;
		case 3:
			mult = easeInOutQuad(time);
		break;
		case 4:
			mult = easeInQuad(time);
		break;
		case 5:
			mult = easeOutQuad(time);
		break;
		case 6:
			mult = easeInOutCubic(time);
		break;
		case 7:
			mult = easeInCubic(time);
		break;
		case 8:
			mult = easeOutCubic(time);
		break;
		case 9:
			mult = easeInOutQuart(time);
		break;
		case 10:
			mult = easeInQuart(time);
		break;
		case 11:
			mult = easeOutQuart(time);
		break;
		case 12:
			mult = easeInOutQuint(time);
		break;
		case 13:
			mult = easeInQuint(time);
		break;
		case 14:
			mult = easeOutQuint(time);
		break;
		case 15:
			mult = easeInOutExpo(time);
		break;
		case 16:
			mult = easeInExpo(time);
		break;
		case 17:
			mult = easeOutExpo(time);
		break;
		case 18:
			mult = easeInOutCirc(time);
		break;
		case 19:
			mult = easeInCirc(time);
		break;
		case 20:
			mult = easeOutCirc(time);
		break;
		case 21:
			mult = easeInOutBack(time);
		break;
		case 22:
			mult = easeInBack(time);
		break;
		case 23:
			mult = easeOutBack(time);
		break;
		case 24:
			mult = easeInOutElastic(time);
		break;
		case 25:
			mult = easeInElastic(time);
		break;
		case 26:
			mult = easeOutElastic(time);
		break;
		case 27:
			mult = easeInOutBounce(time);
		break;
		case 28:
			mult = easeInBounce(time);
		break;
		case 29:
			mult = easeOutBounce(time);
		break;
	}
	return mult;
}
