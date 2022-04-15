#version 460 core

#define MAX_ITERATIONS 300
#define MAX_ITERATIONS_SUN 300

#define RAY_SCATTER_OFFSET_MAX 0.05
//maximum offset for full ray lenght

#define RAY_SCATTER_MAX 0.000333333333
//0.05/MAX_ITERATIONS

in vec2 pos;

layout(location=0) out vec3 fragColor;

uniform vec3 cameraPosition;

uniform vec3 upVec;
uniform vec3 rightVec;
uniform vec3 frontVec;

uniform uint tick;

uint hash( uint x ) {
    x += ( x << 10u );
    x ^= ( x >>  6u );
    x += ( x <<  3u );
    x ^= ( x >> 11u );
    x += ( x << 15u );
    return x;
}

uint hashSmall( uint x ) {
    x += ( x << 10u );
    x ^= ( x >>  6u );
    x += ( x <<  3u );
    x ^= ( x >> 11u );
    return x;
}

uint hash8( uint x ) {
    x += ( x << 10u );
    x ^= ( x >>  6u );
    x += ( x <<  3u );
    x ^= ( x >> 11u );
    x += ( x << 15u );
    return x&0xFFFF;
}

vec3 rngV3(uint seed)
{
    uint x = seed+( seed << 10u );
    x ^= ( x >>  6u );
    x += ( x <<  3u );
    x ^= ( x >> 11u );
	uint z = x;
    x += ( x << 15u );
	
	return vec3(
		x&0xFFFF,(z+seed)&0xFFFF,hash8(seed+3)
	)*(0.0000305185095)-1.0;
}

const vec3 sunPos = vec3(3,5,2);

bool pAABB(vec3 point, vec3 min, vec3 max)
{
	return (point.x >= min.x && point.x <= max.x) &&
         (point.y >= min.y && point.y <= max.y) &&
         (point.z >= min.z && point.z <= max.z);
}

void main()
{
	vec2 absPos = pos*vec2(521107,8693)+vec2(521231,10151);
	uint s = hashSmall(
			tick+
			uint(absPos.x)+
			uint(absPos.y)
		);
	
	if((s%3)!=1)
	{
		discard;
		//fragColor = vec3(0.0);
		//return;
	}
	
	float fov = 1.0;
	vec3 rayDir = mat3(rightVec,upVec,frontVec) * normalize(vec3(pos,fov));//+z = front
	vec3 rayPos = cameraPosition;
	
	vec3 col;
	
	
	float depth = 0.0;
	
	bool hit = false;
	for(int i = 0;i<MAX_ITERATIONS;i++)
	{
		depth+=0.005;
		rayPos+=rayDir*0.005+(
			rngV3(
				s++
			)*RAY_SCATTER_MAX
		);
		
		if(pAABB(rayPos,vec3(0.0,0.0,1.0),vec3(1.0,1.0,20.0)) )
		{
			hit = true;
			break;
		}
		else if(pAABB(rayPos,vec3(0.0,-1.0,-20.0),vec3(1.0,2.0,-0.25)) )
		{
			hit = true;
			break;
		}
		//else if(pAABB(rayPos,sunPos-0.25,sunPos+0.25) )
		//{
		//	fragColor = vec3(0.9,1.0,0.8);
		//	return;
		//}
	}
	if(!hit)
	{
		col = vec3(0.2,0.4,0.8);
	}
	else
	{
		vec3 sunRayPos = rayPos;
		
		vec3 sunRayDir = normalize(sunPos-sunRayPos);
		
		//float sunDepth = 0.0;
		
		bool sunHit = false;
		
		for(int ij = 0;ij<MAX_ITERATIONS_SUN;ij++)
		//in shade
		{
			
			//sunDepth+=0.05;
			sunRayPos+=sunRayDir*0.01;
			
			if(pAABB(sunRayPos,vec3(0.0,0.0,1.0),vec3(1.0,1.0,20.0)) )
			{
				sunHit = true;
				break;
			}
			else if(pAABB(sunRayPos,vec3(0.0,-1.0,-20.0),vec3(1.0,2.0,-0.25)) )
			{
				sunHit = true;
				break;
			}
		}
		if(sunHit)
		{
			col = vec3(0.5,0.57,0.6);
		}
		else
		{
			col = vec3(0.9,0.97,0.99);
		}
	}
	
	fragColor = col;
}