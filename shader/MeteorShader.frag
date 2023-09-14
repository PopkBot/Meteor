#version 130

uniform vec2 u_resolution;
uniform float u_trajectory_scale;
uniform vec2 u_mouse;
uniform vec3 u_pos;

uniform vec3 u_planetPos;
uniform vec3 u_planetCol;
uniform float u_planetRad;

uniform vec3 u_sunVec;

uniform vec3 u_satPos;
uniform vec3 u_satCol;
uniform vec3 u_satVecX;
uniform vec3 u_satVecY;
uniform vec3 u_satVecZ;

uniform vec2 screen1_pos;
uniform vec2 screen1_dim;

uniform float isDrawOrbitalCS;
uniform vec3 orbX;
uniform vec3 orbY;
uniform vec3 orbZ;

uniform vec3 u_elU;
uniform vec3 u_elV;
uniform vec3 u_elC;

uniform vec3 u_targetW;
uniform vec3 u_W;
uniform vec3 u_moment;

uniform vec3 u_Avec;
uniform vec3 u_CDvec;

//uniform sampler2D u_textureGrass;

vec2 minObj;
vec3 norm;
const float MAX_DIST = 999999.0;
const int MAX_REF = 8;
vec3 light = normalize(u_sunVec);
float trackFlag =0;
vec2 uv;

//ro коорд камеры	(единич)
//rd навправ луча	(единич)
mat2 rot(float a){					//матрица
	float s=sin(a);
	float c=cos(a);
	return mat2(c,-s,s,c);
}

float board( in vec2 p, in vec2 center, in float a,in float b )
{
    
    vec2 pc = p-center;
    if(abs(pc.x)<a && abs(pc.y)<b){
    	return 1;
    }
    return -1;

}


vec2 boxIntersection(in vec3  ro,in vec3  rd,in vec3 boxSize, out vec3 outNormal ) 
{
    vec3 m = 1.0/rd; // can precompute if traversing a set of aligned boxes
    vec3 n = m*ro;   // can precompute if traversing a set of aligned boxes
    vec3 k = abs(m)*boxSize;
    vec3 t1 = -n - k;
    vec3 t2 = -n + k;
    float tN = max( max( t1.x, t1.y ), t1.z );
    float tF = min( min( t2.x, t2.y ), t2.z );
    if( tN>tF || tF<0.0) return vec2(-1.0); // no intersection
    outNormal = -sign(rd)*step(t1.yzx,t1.xyz)*step(t1.zxy,t1.xyz);
    return vec2( tN, tF );
}

vec2 sphIntersect( in vec3 ro, in vec3 rd, float ra )
{
   // vec3 oc = ro - ce;
    float b = dot( ro, rd );
    float c = dot( ro, ro ) - ra*ra;
    float h = b*b - c;
    if( h<0.0 ) return vec2(-1.0); // no intersection
    h = sqrt( h );
    return vec2( -b-h, -b+h );
}

vec2 rectIntersect( in vec3 ro, in vec3 rd, vec3 c, vec3 u, vec3 v )
{
    vec3  q =ro-c;
    vec3  n = cross(u,v);
    float t = -dot(n,q)/dot(rd,n);
    float r =  dot(normalize(u),q + rd*t);
    float s =  dot(normalize (v),q + rd*t);
    //if( ((r*r/(length(u)*length(u))+s*s/(length(v)*length(v)) )>1.0)) return vec2(-1.0); //no intersection
    if(abs(r)>length(u) || abs(s)>length(v))return vec2(-1.0); 
    return vec2(t,s);
}

vec2 ellORBIntersect( in vec3 ro, in vec3 rd, vec3 c, vec3 u, vec3 v )
{
    vec3  q =ro-c;
    vec3  n = cross(u,v);
    float t = -dot(n,q)/dot(rd,n);
    float r =  dot(normalize(u),q + rd*t);
    float s =  dot(normalize (v),q + rd*t);
    if( ((r*r/(length(u)*length(u))+s*s/(length(v)*length(v)) )>1.0) || 
    	((r*r/(length(u)*length(u))+s*s/(length(v)*length(v)) )<u_trajectory_scale)) return vec2(-1.0); //no intersection
    return vec2(t,s);
}

vec3 ellNormal( in vec3 u, vec3 v )
{
    return normalize( cross(u,v) );
}


vec3 getsSky(vec3 rd){
	vec3 col=vec3(0,0,0);
	vec3 sun=vec3(1,1,1.0);
	sun*=max(0.0,pow(dot(rd,light),1000.0));
	return clamp(sun+col,0.0,1.0);
}

float dot2( in vec3 v ) { return dot(v,v); }

float mod2(in float a,in float b){return a-(b*floor(a/b)); }

vec2 coneIntersect( in vec3  ro, in vec3  rd, in vec3  pa, in vec3  pb, in float ra, in float rb,out vec3 coneN )
{
    vec3  ba = pb - pa;
    vec3  oa = ro - pa;
    vec3  ob = ro - pb;
    float m0 = dot(ba,ba);
    float m1 = dot(oa,ba);
    float m2 = dot(rd,ba);
    float m3 = dot(rd,oa);
    float m5 = dot(oa,oa);
    float m9 = dot(ob,ba); 
    
    // caps
    if( m1<0.0 )
    {
        if( dot2(oa*m2-rd*m1)<(ra*ra*m2*m2) ) {// delayed division
        	coneN=-ba*inversesqrt(m0);
            return vec2(-m1/m2,dot2(-ba*inversesqrt(m0)));
        }
    }
    else if( m9>0.0 )
    {
    	float t = -m9/m2;                     // NOT delayed division
        if( dot2(ob+rd*t)<(rb*rb) ){
        	coneN=ba*inversesqrt(m0);
            return vec2(t,dot2(ba*inversesqrt(m0)));
        }
    }
    
    // body
    float rr = ra - rb;
    float hy = m0 + rr*rr;
    float k2 = m0*m0    - m2*m2*hy;
    float k1 = m0*m0*m3 - m1*m2*hy + m0*ra*(rr*m2*1.0        );
    float k0 = m0*m0*m5 - m1*m1*hy + m0*ra*(rr*m1*2.0 - m0*ra);
    float h = k1*k1 - k2*k0;
    if( h<0.0 ) return vec2(-1.0); //no intersection
    float t = (-k1-sqrt(h))/k2;
    float y = m1 + t*m2;
    if( y<0.0 || y>m0 ) return vec2(-1.0); //no intersection
    coneN=normalize(m0*(m0*(oa+t*rd)+rr*ba*ra)-ba*hy*y);
    return vec2(t,dot2( normalize(m0*(m0*(oa+t*rd)+rr*ba*ra)-ba*hy*y)));
}


vec2 iRoundedCone( in vec3 ro, in vec3 rd, in vec3 pa, in vec3 pb, in float ra, in float rb ,out vec3 rcN)
{
    vec3  ba = pb - pa;
    vec3  oa = ro - pa;
    vec3  ob = ro - pb;
    float rr = ra - rb;
    float m0 = dot(ba,ba);
    float m1 = dot(ba,oa);
    float m2 = dot(ba,rd);
    float m3 = dot(rd,oa);
    float m5 = dot(oa,oa);
    float m6 = dot(ob,rd);
    float m7 = dot(ob,ob);
    
    // body
    float d2 = m0-rr*rr;
    float k2 = d2    - m2*m2;
    float k1 = d2*m3 - m1*m2 + m2*rr*ra;
    float k0 = d2*m5 - m1*m1 + m1*rr*ra*2.0 - m0*ra*ra;
    float h = k1*k1 - k0*k2;
    if( h<0.0) return vec2(-1.0);
    float t = (-sqrt(h)-k1)/k2;
  //if( t<0.0 ) return vec4(-1.0);
    float y = m1 - ra*rr + t*m2;
    if( y>0.0 && y<d2 )
    { 
    	rcN=normalize(d2*(oa+t*rd)-ba*y);

    	return vec2(t, dot2(normalize(d2*(oa+t*rd)-ba*y)));
    }

    // caps
    float h1 = m3*m3 - m5 + ra*ra;
    float h2 = m6*m6 - m7 + rb*rb;
    if( max(h1,h2)<0.0 ) return vec2(-1.0);
    vec2 r = vec2(1e20);
    if( h1>0.0 )
    {        
    	t = -m3 - sqrt( h1 );
    	rcN=(oa+t*rd)/ra;
        r = vec2( t,dot2( (oa+t*rd)/ra ));
    }
    if( h2>0.0 )
    {
    	t = -m6 - sqrt( h2 );
        if( t<r.x ){
        	rcN=	(ob+t*rd)/rb;
        	r = vec2( t, dot2((ob+t*rd)/rb ));
        }
    }
    return r;
}

float plaIntersect( in vec3 ro, in vec3 rd, in vec4 p )
{
    return -(dot(ro,p.xyz)+p.w)/dot(rd,p.xyz);
}	


// "p" point being textured
// "n" surface normal at "p"
// "k" controls the sharpness of the blending in the transitions areas
// "s" texture sampler
vec4 boxmap( in sampler2D s, in vec3 p, in vec3 n, in float k )
{
    // project+fetch
    vec4 x = texture( s, p.yz );
    vec4 y = texture( s, p.zx );
    vec4 z = texture( s, p.xy );
    
    // blend factors
    vec3 w = pow( abs(n), vec3(k) );
    // blend and return
    return (x*w.x + y*w.y + z*w.z) / (w.x + w.y + w.z);
}




vec3 castRay(vec3 ro,vec3 rd)
{

	vec3 col;
	minObj=vec2(MAX_DIST);
	vec2 obj;
	vec3 n;
	trackFlag =0.0;


	vec3 spherePos=u_planetPos;
	obj=sphIntersect(ro-spherePos,rd,u_planetRad);
	if(obj.x>0.0 && obj.x<minObj.x){
		minObj=obj;
		vec3 objPos=ro+rd*obj.x;
		n=objPos-spherePos;
		col=u_planetCol;
		trackFlag=2;
	}
	
	

		vec3 coneN;
		vec3 pa=u_satPos;
		vec3 pb=u_satPos+u_satVecZ*3;
		float ra=0.5;
		float rb=0.5;
		//vec4 coneObj = coneIntersect( in vec3  ro, in vec3  rd, in vec3  pa, in vec3  pb, in float ra, in float rb );
		obj=coneIntersect(  ro,   rd, pa, pb,  ra, rb , coneN);
		if(obj.x>0.0 && obj.x<minObj.x){
			minObj=obj;
			n=-coneN;
			col=u_satCol;	
			trackFlag=0;
		}

		vec3 coneN2;
		pa=u_satPos;
		pb=u_satPos+u_satVecZ*(-0.5);
		ra=0.7;
		rb=0.7;
		//vec4 coneObj = coneIntersect( in vec3  ro, in vec3  rd, in vec3  pa, in vec3  pb, in float ra, in float rb );
		obj=coneIntersect(  ro,   rd, pa, pb,  ra, rb , coneN2);
		if(obj.x>0.0 && obj.x<minObj.x){
			minObj=obj;
			n=-coneN2;
			col=u_satCol;	
			trackFlag=0;
		}


		/*vec3 planeNormal = u_satVecY;
		vec4 solarPan1 = vec4(planeNormal,1);
		vec3 panelPos=u_satPos+u_satVecZ;
		obj =vec2( plaIntersect(ro-u_satPos, rd, solarPan1));
		if(obj.x > 0.0 && obj.x < minObj.x) {
			minObj = obj;
			n = planeNormal;
			col=vec3(0,0.5,0);
		}*/


		obj=rectIntersect(  ro, rd, u_satPos+u_satVecZ+u_satVecX*2.5,u_satVecX*2, u_satVecZ);
		if(obj.x>0.0 && obj.x<minObj.x){
		minObj=obj;
		vec3 objPos=ro+rd*obj.x;
		n=ellNormal(u_satVecX,u_satVecZ);
		col=vec3(0.05,0.1,0.3);
		//col=vec3(1.0,1.0,0);
		trackFlag=3;
		}


		obj=rectIntersect(  ro, rd, u_satPos+u_satVecZ-u_satVecX*2.5,u_satVecX*2, u_satVecZ);
		if(obj.x>0.0 && obj.x<minObj.x){
		minObj=obj;
		vec3 objPos=ro+rd*obj.x;
		n=ellNormal(u_satVecX,u_satVecZ);
		col=vec3(0.05,0.1,0.3);
		//col=vec3(1.0,1.0,0);
		trackFlag=3;
		}
			


		
		vec3 rconeNX;
		vec3 rcpa=u_satPos+u_satVecX*2;
		vec3 rcpb=u_satPos;
		float rcra=0.05;
		float rcrb=0.05;
		obj=iRoundedCone(  ro,  rd,  rcpa,  rcpb,  rcra,  rcrb ,  rconeNX);
		if(obj.x>0.0 && obj.x<minObj.x){
			minObj=obj;
			n=rconeNX;
			col=vec3(1,0,0);	
			trackFlag=4;
		}

		vec3 rconeNY;
		 rcpa=u_satPos+u_satVecY*2;
		 rcpb=u_satPos;
		 rcra=0.05;
		 rcrb=0.05;
		obj=iRoundedCone(  ro,  rd,  rcpa,  rcpb,  rcra,  rcrb ,  rconeNY);
		if(obj.x>0.0 && obj.x<minObj.x){
			minObj=obj;
			n=rconeNY;
			col=vec3(0,1,0);
			trackFlag=4;
		}

		vec3 rconeNZ;
		 rcpa=u_satPos+u_satVecZ*4;
		 rcpb=u_satPos;
		 rcra=0.05;
		 rcrb=0.05;
		obj=iRoundedCone(  ro,  rd,  rcpa,  rcpb,  rcra,  rcrb ,  rconeNZ);
		if(obj.x>0.0 && obj.x<minObj.x){
			minObj=obj;
			n=rconeNZ;
			col=vec3(0,0,1);
			trackFlag=4;	
		}	


		if(isDrawOrbitalCS>=1){
				vec3 rconeNXO;
			rcpa=u_satPos+orbX*4;
			rcpb=u_satPos;
			rcra=0.05;
			rcrb=0.05;
			obj=iRoundedCone(  ro,  rd,  rcpa,  rcpb,  rcra,  rcrb ,  rconeNXO);
			if(obj.x>0.0 && obj.x<minObj.x){
				minObj=obj;
				n=rconeNXO;
				col=vec3(1,0.16,0.33);	
				trackFlag=5;
			}

			vec3 rconeNYO;
			 rcpa=u_satPos+orbY*4;
			 rcpb=u_satPos;
			 rcra=0.05;
			 rcrb=0.05;
			obj=iRoundedCone(  ro,  rd,  rcpa,  rcpb,  rcra,  rcrb ,  rconeNYO);
			if(obj.x>0.0 && obj.x<minObj.x){
				minObj=obj;
				n=rconeNYO;
				col=vec3(0.588,1,0.588);
				trackFlag=5;
			}

			vec3 rconeNZO;
			 rcpa=u_satPos+orbZ*4;
			 rcpb=u_satPos;
			 rcra=0.05;
			 rcrb=0.05;
			obj=iRoundedCone(  ro,  rd,  rcpa,  rcpb,  rcra,  rcrb ,  rconeNZO);
			if(obj.x>0.0 && obj.x<minObj.x){
				minObj=obj;
				n=rconeNZO;
				col=vec3(0.26,0.67,1);
				trackFlag=5;	
			}	
		}

/*
		vec3 targetW;
		 rcpa=u_satPos;
		 rcpb=u_planetPos;
		 rcra=5;
		 rcrb=5;
		obj=iRoundedCone(  ro,  rd,  rcpa,  rcpb,  rcra,  rcrb ,  targetW);
		if(obj.x>0.0 && obj.x<minObj.x){
			minObj=obj;
			n=targetW;
			col=vec3(1,0.16,0.33);//розовый
			trackFlag=5;	
		}	

		vec3 w_vec;
		 rcpa=u_satPos+u_W*4;
		 rcpb=u_satPos;
		 rcra=0.05;
		 rcrb=0.05;
		obj=iRoundedCone(  ro,  rd,  rcpa,  rcpb,  rcra,  rcrb ,  w_vec);
		if(obj.x>0.0 && obj.x<minObj.x){
			minObj=obj;
			n=w_vec;
			col=vec3(0.26,0.67,1);//голубой
			trackFlag=5;	
		}	

		vec3 moment;
		 rcpa=u_satPos+u_moment*4;
		 rcpb=u_satPos;
		 rcra=0.05;
		 rcrb=0.05;
		obj=iRoundedCone(  ro,  rd,  rcpa,  rcpb,  rcra,  rcrb ,  moment);
		if(obj.x>0.0 && obj.x<minObj.x){
			minObj=obj;
			n=moment;
			col=vec3(0.588,1,0.588);//салатовый
			trackFlag=5;	
		}*/	
		

		obj=ellORBIntersect(  ro, rd, u_elC,u_elU, u_elV);
		if(obj.x>0.0 && obj.x<minObj.x){
		minObj=obj;
		vec3 objPos=ro+rd*obj.x;
		n=ellNormal(u_elU,u_elV);
		col=vec3(1,0,0);
		//col=vec3(1.0,1.0,0);
		trackFlag=4;
		}

	
		
	if(minObj.x == MAX_DIST) return getsSky(rd);
	if(trackFlag==1){
	float diffuse =max(0.001,dot(light,n)*0.008+0.0001);
	float specular= max(0.001,pow( dot(reflect(rd,n),light),0.01));
	col *= mix(diffuse,specular,0.5);
	}
	else if(trackFlag==2){
	float diffuse =max(0.01,dot(light,n)*0.8+0.1);
	float specular= max(0.01,pow( dot(reflect(rd,n),light),0.0001));
	col *= mix(diffuse,specular,0.99);
	}
	else if(trackFlag==4){
		float diffuse =0;//max(1,dot(light,n)*0.1+0.5);
		float specular= 1;//max(1,pow( dot(reflect(rd,n),light),0.5));
		col *= mix(diffuse,specular,100);
	}
	else if(trackFlag==5){
		//float diffuse =0;//max(1,dot(light,n)*0.1+0.5);
		//float specular= 1;//max(1,pow( dot(reflect(rd,n),light),0.5));
		//col *= mix(diffuse,specular,100);
	}
//col=col+vec3(0.001,0.001,0.001);
ro += rd * (minObj.x-0.001);
norm=n;
return col;

}



vec3 traceRay(vec3 ro,vec3 rd){

vec3 col=castRay(ro,rd);


if(col.x==-1.0) return getsSky(rd);
vec3 lightDir=light;
ro += rd*(minObj.x-0.001);
rd=norm;

if(dot(rd,light)>0){
if(castRay(ro,lightDir).x != -1.0) col *= 0.1;


}
return col;

}


vec3 ray(in vec2 p, in vec3 col){


	float dist;

	float a = screen1_dim.x;
	float b = screen1_dim.y;
	vec2  pa = vec2(-u_resolution.x*0.6,u_resolution.y*0.75);

	dist=board( p, pa , a, b );
	if(dist>0)	{
		col=vec3(0.2,0.2,0.2);
	}
	return col;
}








void main(){
	uv = (gl_TexCoord[0].xy-0.5) * u_resolution / u_resolution.y;//(gl_TexCoord[0].xy-0.5)*u_resolution/u_resolution.y
	vec3 rayOrigin = u_pos;
	vec3 rayDirection = normalize(vec3(1.0,uv));
	rayDirection.zx *= rot(-u_mouse.y);
	rayDirection.xy *= rot(u_mouse.x);
	vec3 col = traceRay(rayOrigin,rayDirection);
	

	
	vec2 p = (2.0*gl_FragCoord.xy-u_resolution.xy)* u_resolution/u_resolution.y;
	//p+=vec2(0,500);
    //col = ray(p,col);

    col.r=pow(col.r,0.45);
	col.g=pow(col.g,0.45);
	col.b=pow(col.b,0.45);
	gl_FragColor = vec4(col, 1);

	//gradientColor( gl_FragColor,uv);
	//gl_FragColor=vec4(uv,0.0,1.0);
}

