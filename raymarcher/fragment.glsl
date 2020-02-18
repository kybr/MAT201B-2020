// Created by inigo quilez - 2015
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0

// Adapted by Mert Toka - 2020

#version 400

layout(location = 0) out vec4 fragmentColor;

uniform float time;
uniform vec2 res;
uniform float count;
uniform float morph;

// http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
float sdSphere( in vec3 p, in vec4 s )
{
    return length(p-s.xyz) - s.w;
}

// http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
float sdEllipsoid( in vec3 p, in vec3 c, in vec3 r )
{
    return (length( (p-c)/r ) - 1.0) * min(min(r.x,r.y),r.z);
}

// http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
float sdCircle( in vec2 p, in vec2 c, in float r )
{
    return length(p-c) - r;
}

// http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
float sdTorus( vec3 p, vec2 t )
{
    return length( vec2(length(p.xz)-t.x,p.y) )-t.y;
}

// http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
float sdCapsule( vec3 p, vec3 a, vec3 b, float r )
{
	vec3 pa = p-a, ba = b-a;
	float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
	return length( pa - ba*h ) - r;
}

// http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
vec2 udSegment( vec3 p, vec3 a, vec3 b )
{
	vec3 pa = p-a, ba = b-a;
	float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
	return vec2( length( pa - ba*h ), h );
}

// http://research.microsoft.com/en-us/um/people/hoppe/ravg.pdf
float det( vec2 a, vec2 b ) { return a.x*b.y-b.x*a.y; }
vec3 getClosest( vec2 b0, vec2 b1, vec2 b2 ) 
{
    float a =     det(b0,b2);
    float b = 2.0*det(b1,b0);
    float d = 2.0*det(b2,b1);
    float f = b*d - a*a;
    vec2  d21 = b2-b1;
    vec2  d10 = b1-b0;
    vec2  d20 = b2-b0;
    vec2  gf = 2.0*(b*d21+d*d10+a*d20); gf = vec2(gf.y,-gf.x);
    vec2  pp = -f*gf/dot(gf,gf);
    vec2  d0p = b0-pp;
    float ap = det(d0p,d20);
    float bp = 2.0*det(d10,d0p);
    float t = clamp( (ap+bp)/(2.0*a+b+d), 0.0 ,1.0 );
    return vec3( mix(mix(b0,b1,t), mix(b1,b2,t),t), t );
}

vec4 sdBezier( vec3 a, vec3 b, vec3 c, vec3 p )
{
	vec3 w = normalize( cross( c-b, a-b ) );
	vec3 u = normalize( c-b );
	vec3 v =          ( cross( w, u ) );

	vec2 a2 = vec2( dot(a-b,u), dot(a-b,v) );
	vec2 b2 = vec2( 0.0 );
	vec2 c2 = vec2( dot(c-b,u), dot(c-b,v) );
	vec3 p3 = vec3( dot(p-b,u), dot(p-b,v), dot(p-b,w) );

	vec3 cp = getClosest( a2-p3.xy, b2-p3.xy, c2-p3.xy );

	return vec4( sqrt(dot(cp.xy,cp.xy)+p3.z*p3.z), cp.z, length(cp.xy), p3.z );
}

// http://iquilezles.org/www/articles/smin/smin.htm
float smin( float a, float b, float k )
{
	float h = clamp( 0.5 + 0.5*(b-a)/k, 0.0, 1.0 );
	return mix( b, a, h ) - k*h*(1.0-h);
}

// http://iquilezles.org/www/articles/smin/smin.htm
float smax( float a, float b, float k )
{
	float h = clamp( 0.5 + 0.5*(b-a)/k, 0.0, 1.0 );
	return mix( a, b, h ) + k*h*(1.0-h);
}

// http://iquilezles.org/www/articles/smin/smin.htm
vec3 smax( vec3 a, vec3 b, float k )
{
	vec3 h = clamp( 0.5 + 0.5*(b-a)/k, 0.0, 1.0 );
	return mix( a, b, h ) + k*h*(1.0-h);
}

//---------------------------------------------------------------------------

float hash1( float n )
{
    return fract(sin(n)*43758.5453123);
}

vec3 hash3( float n )
{
    return fract(sin(n+vec3(0.0,13.1,31.3))*158.5453123);
}

vec3 forwardSF( float i, float n) 
{
    const float PI  = 3.141592653589793238;
    const float PHI = 1.618033988749894848;
    float phi = 2.0*PI*fract(i/PHI);
    float zi = 1.0 - (2.0*i+1.0)/n;
    float sinTheta = sqrt( 1.0 - zi*zi);
    return vec3( cos(phi)*sinTheta, sin(phi)*sinTheta, zi);
}

//---------------------------------------------------------------------------

const float pi = 3.1415927;

//---------------------------------------------------------------------------

// map function
float map(float value, float min1, float max1, float min2, float max2) {
  return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

// 
float rampForever(float min, float max) {
    float rateControl = max;
    float y = fract(time/rateControl);
    return map(y, 0.0, 1.0, min, max);
}

float mapSmallSphere( vec3 p, out vec4 matInfo, float size, float theta, float phi, float r) 
{
    // check size    
    if(size < 0.0) size = abs(size);
    if(size > 0.6) size -= 0.6;

    // use size to calculate duration of fall
    float duration = map(size, 0.0, 0.5, 7.0, 2.0);

    // spherical to euclidean coords
    vec3 pos = vec3(r*sin(theta)*cos(phi),
                    r*sin(theta)*sin(phi),
                    r*cos(theta));    
    vec3 dir = -pos;

    // disturb trajectories
    vec3 disturb = 0.4 * vec3(sin(0.2 * time),  cos(0.9 * p.y), sin(0.3 * p.z));

    // distance to origin
    float dist = length(dir);
    float v = dist/duration;

    // continous ramp function 
    float k = rampForever(0.0, duration);

    // calculate next position
    vec3 n_pos = dir - k*v*normalize(dir + disturb) ;
    
    // render the ball
    float ball = sdSphere(p, vec4(n_pos, size));

    return ball;
}

float mapSmallSpheres( vec3 p, out vec4 matInfo ) 
{
    // bunch of spheres -- smin combines them
    //                                         [ 0,pi], [0,2*pi]
    float res = mapSmallSphere(p, matInfo, 0.26, pi*.6, pi*0.1,  25. );
    for(float i = 0.0; i < count; i++) {
        res = smin(res, 
                   mapSmallSphere(p, matInfo, 
                                  hash1(i)/2.0+0.05,      // radius of the sphere 
                                  pi*.6, i/count * 2.0*pi,  // theta, phi   -- spherical coord
                                  25. ),                   // r            -- spherical coord 
                   0.02);
    }

    return res;
}

vec2 mapOpaque( vec3 p, out vec4 matInfo )
{
    matInfo = vec4(0.0);

    float scale = 1.0 + log2(time+1.0) * map(count, 1.0, 50.0, 0.1, 0.2);
    vec2 res = vec2(sdSphere(p+(0.3 * vec3(sin(0.2 * time),  cos(0.9 * p.y), sin(0.3 * p.z))), vec4(0.0,.0,.0, scale)), 1.0);

    float d2 = mapSmallSpheres(p, matInfo);
    if(d2 < res.x)  { res = vec2(d2, 2.0); }

    res.x = smin(res.x, d2, morph);

    return res;
}

// http://iquilezles.org/www/articles/normalsSDF/normalsSDF.htm
vec3 calcNormalOpaque( in vec3 pos, in float eps )
{
    vec4 kk;
    // inspired by klems - a way to prevent the compiler from inlining map() 4 times
    vec3 n = vec3(0.0);
    for( int i=0; i<4; i++ )
    {
        vec3 e = 0.5773*(2.0*vec3((((i+3)>>1)&1),((i>>1)&1),(i&1))-1.0);
        n += e*mapOpaque(pos+eps*e,kk).x;
    }
    return normalize(n); 
}


//=========================================================================

float calcAO( in vec3 pos, in vec3 nor )
{
    vec4 kk;
    float ao = 0.0;
    for( int i=0; i<32; i++ )
    {
        vec3 ap = forwardSF( float(i), 32.0 );
        float h = hash1(float(i));
		ap *= sign( dot(ap,nor) ) * h*0.1;
        ao += clamp( mapOpaque( pos + nor*0.01 + ap, kk ).x*3.0, 0.0, 1.0 );
    }
	ao /= 32.0;
	
    return clamp( ao*6.0, 0.0, 1.0 );
}

// http://iquilezles.org/www/articles/rmshadows/rmshadows.htm
float calcSoftShadow( in vec3 ro, in vec3 rd, float k )
{
    vec4 kk;    
    float res = 1.0;
    float t = 0.0;
    for( int i=0; i<32; i++ )
    {
        float h = mapOpaque(ro + rd*t, kk ).x;
        res = min( res, smoothstep(0.0,1.0,k*h/t) );
        t += clamp( h, 0.04, 0.1 );
		if( res<0.01 ) break;
    }
    return clamp(res,0.0,1.0);
}

const vec3 sunDir = normalize( vec3(0.2,0.8,0.02) );
vec3 shadeOpaque( in vec3 ro, in vec3 rd, in float t, in float m, in vec4 matInfo )
{
    float eps = 0.002;
    
    vec3 pos = ro + t*rd;
    vec3 nor = calcNormalOpaque( pos, eps );

    vec3 mateD = vec3(0.0);
    vec3 mateE = vec3(0.0);

    float focc = 1.0;
    float fsha = 1.0;

    // central sphere
    if( m<1.5 )
    {
        mateD = vec3(1.35,0.71,0.13)*0.15;
        
        float fre = clamp(1.0+dot(nor,rd), 0.0, 1.0 );
        mateD += 0.2*fre*vec3(0.6,0.5,0.1);
        
        mateE = 0.5*vec3(0.4,2.0, 0.43);
    }
    // balls
    else if(m < 2.5) {
        mateD = vec3(0.6,0.7,0.0)*0.2;

        float fre = clamp(1.0+dot(nor,rd), 0.0, 1.0 );
        mateD += 0.2*fre*vec3(1.0,0.2,0.1);
        
        mateE = 0.5*vec3(0.9,0.1,0.03);
    }
    
    // shading parameters
    vec3 hal = normalize( sunDir-rd );
    float fre = clamp(1.0+dot(nor,rd), 0.0, 1.0 );
    float occ = calcAO( pos, nor )*focc;
    
    float dif1 = clamp( dot(nor,sunDir), 0.0, 1.0 );
    float sha = calcSoftShadow( pos, sunDir, 20.0 ); 
    dif1 *= sha*fsha;
    float spe1 = clamp( dot(nor,hal), 0.0, 1.0 );

    float bou = clamp( 0.3-0.7*nor.y, 0.0, 1.0 );

    // illumination
    vec3 col = vec3(0.0);
    col += 7.0*vec3(1.7,1.2,0.6)*dif1*2.0;               // sun
    col += 4.0*vec3(0.2,1.2,1.6)*occ*(0.5+0.5*nor.y);    // sky
    col += 1.8*vec3(0.1,2.0,0.1)*bou*occ;                // bounce

    col *= mateD;

    col = pow(col,vec3(0.6,0.8,1.0));

    col += mateE;

    return col;        
}

//--------------------------------------------

vec2 intersectOpaque( in vec3 ro, in vec3 rd, const float mindist, const float maxdist, out vec4 matInfo )
{
    vec2 res = vec2(-1.0);
    
    float t = mindist;
    for( int i=0; i<64; i++ )
    {
        vec3 p = ro + t*rd;
        vec2 h = mapOpaque( p, matInfo );
        res = vec2(t,h.y);

        if( h.x<(0.001*t) ||  t>maxdist ) break;
        
        t += h.x*0.9;
    }
	return res;
}

vec3 render( in vec3 ro, in vec3 rd, in vec2 q )
{
    //-----------------------------

    vec3 col = vec3(0.0);
    
    //-----------------------------
    
    float mindist = 0.5;
    float maxdist = 20.0;

    vec4 matInfo;
    vec2 tm = intersectOpaque( ro, rd, mindist, maxdist, matInfo );
    if( tm.y>-0.5 && tm.x < maxdist )
    {
        col = shadeOpaque( ro, rd, tm.x, tm.y, matInfo );
        maxdist = tm.x;
    }

    //-----------------------------
    
    // float sun = clamp(dot(rd,sunDir),0.0,1.0);
    // col += 1.0*vec3(1.5,0.8,0.7)*pow(sun,2.0);

    //-----------------------------

    col = pow( col, vec3(0.45) );
    
    col = vec3(1.05,1.0,1.0)*col*(0.7+0.3*col*max(3.0-2.0*col,0.0)) + vec3(0.0,0.0,0.04);

    col *= 0.3 + 0.7*pow(16.0*q.x*q.y*(1.0-q.x)*(1.0-q.y),0.1);

    return clamp( col, 0.0, 1.0 );
}

mat3 setCamera( in vec3 ro, in vec3 rt )
{
    vec3 w = normalize(ro-rt);
    
    float m = sqrt(1.0-w.y*w.y);
    
    return mat3( w.z,     0.0, -w.x, 
                 0.0,     m*m, -w.z*w.y,
                 w.x*m, w.y*m,  w.z*m );
}

void main()
{	
    vec2  p = (2.0*gl_FragCoord.xy-res.xy)/res.y;
    vec2  q = gl_FragCoord.xy/res.xy;

    vec3  ro = vec3(0.0,0.0,3.0);
    vec3  ta = vec3(0.0,0.0,0.0);
    mat3  ca = setCamera( ro, ta );
    vec3  rd = normalize( ca * vec3(p,-0.5) );

    vec3 col = render( ro, rd, q );
    
    fragmentColor = vec4( col, 1.0 );
}
