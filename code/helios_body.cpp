
function f64 SolveKeplersEquations(f64 meanAnomaly, f64 eccentricity)
{
  f64 eccentricAnomaly = meanAnomaly; // Initial guess
  f64 precisionTolerance = 1e-12;
  s32 maxSteps = 100;
  
  for (s32 i = 0; i < maxSteps; i++)
  {
    // f(E) = E - e*sin(E) - M
    f64 functionValue = eccentricAnomaly - (eccentricity * SinF64(eccentricAnomaly)) - meanAnomaly;
    
    // f'(E) = 1 - e*cos(E)
    f64 derivativeValue = 1.0 - (eccentricity * CosF64(eccentricAnomaly));
    
    f64 correction = functionValue / derivativeValue;
    eccentricAnomaly -= correction;
    
    if (AbsF64(correction) < precisionTolerance)
    {
      break; // Successfully converged!
    }
  }
  
  return eccentricAnomaly;
}

function void 
UpdateBodyOrbit(keplerian_body* body, f64 deltaTime)
{
  f64 semiMajorCubed = body->semiMajorAxis * body->semiMajorAxis * body->semiMajorAxis;
  //f64 meanMotion = SqrtF64(body->gravityParameter / semiMajorCubed);
  
  body->meanAnomaly += body->meanMotion * deltaTime;
  
  body->meanAnomaly = ModF64(body->meanAnomaly, 2.0 * PiF64);
  if(body->meanAnomaly < 0)
  {
    body->meanAnomaly += 2.0 * PiF64;
  }
  
  f64 eccentricAnomaly = SolveKeplersEquations(body->meanAnomaly, body->eccentricity);
  
  f64 cosE = CosF64(eccentricAnomaly);
  f64 sinE = SinF64(eccentricAnomaly);
  
  f64 localX = (body->semiMajorAxis * cosE) - (body->semiMajorAxis * body->eccentricity);
  f64 localY = body->semiMinorAxis * sinE;
  f64 cosOmega = CosF64(body->argumentOfPeriapsis);
  f64 sinOmega = SinF64(body->argumentOfPeriapsis);
  
  body->position.x = (localX * cosOmega) - (localY * sinOmega);
  body->position.y = -((localX * sinOmega) + (localY * cosOmega));
}

function void
GenerateSolarSystem(memory_arena* arena, star_system* systems, u32 systemID, f32 metersPerHU)
{
  star_system* system = &systems[systemID];
  system->systemID = systemID;
  
  body_generation_info systemGenBodies[] =
  {
#define BODY(name,type,isRoot,...) {HS_Stringify(name),system_body_type::##type,isRoot, __VA_ARGS__},
#include "helios_bodygen_table.inl"
#undef BODY
  };
  
  system_generation_info systemGenInfo = {};
  
  if(systemID == 0)
  {
    systemGenInfo.systemGenBodies = systemGenBodies;
    systemGenInfo.systemGenBodyCount = HS_ArrayCount(systemGenBodies);
  }
  else
  {
    // TODO(Sebas): Generate Random System
    //GenerateRandomSystem(&systemGenInfo);
    systemGenInfo.systemGenBodies = systemGenBodies;
    systemGenInfo.systemGenBodyCount = HS_ArrayCount(systemGenBodies);
  }
  
  system->bodyCount = systemGenInfo.systemGenBodyCount;
  system->bodies = PushType(arena, system_body, system->bodyCount);
  
  u32 lastStarID = 0;
  u32 lastPlanetID = 0;
  u32 lastMoonID = 0;
  u32 lastAsteroidBeltID = 0;
  
  for(u32 bodyID = 0;
      bodyID < system->bodyCount;
      ++bodyID)
  {
    system_body* body = &system->bodies[bodyID];
    body_generation_info* genBody = &systemGenInfo.systemGenBodies[bodyID];
    body->bodyID = bodyID;
    body->type = genBody->type;
    body->name = genBody->name;
    body->colour = genBody->colour;
    body->shouldRender = true;
    switch(body->type)
    {
      case system_body_type::Star:
      {
        body->parentID = 0;
        lastStarID = body->bodyID;
      } break;
      case system_body_type::Planet:
      {
        body->parentID = lastStarID;
        lastPlanetID = body->bodyID;
      } break;
      case system_body_type::Moon:
      {
        body->parentID = lastPlanetID;
        lastMoonID = body->bodyID;
      } break;
      case system_body_type::AsteroidBelt:
      {
        body->parentID = lastStarID;
        lastAsteroidBeltID = body->bodyID;
      } break;
    }
    system_body* parentBody = &system->bodies[body->parentID];
    body->mass = genBody->mass;
    body->radiusHU = genBody->radiusM / metersPerHU;
    keplerian_body* keplerBody = &body->keplerBody;
    // TODO(Sebas): Binary Star system Calculations
    if(body->parentID != body->bodyID)
    {
      
      keplerBody->semiMajorAxis = genBody->avgDistanceFromParent;
      keplerBody->eccentricity = genBody->eccentricity;
      keplerBody->semiMinorAxis = CalculateBodySemiMinorAxis(keplerBody->semiMajorAxis, 
                                                             keplerBody->eccentricity);
      
      keplerBody->meanMotion = CalculateBodyMeanMotion(parentBody->mass, 
                                                       body->mass, 
                                                       keplerBody->semiMajorAxis);
      keplerBody->argumentOfPeriapsis = RadsFromDegreesF64(genBody->argumentOfPeriapsisDeg);
    }
  }
}