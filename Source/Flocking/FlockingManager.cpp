#include "FlockingManager.h"

#define AGENT_COUNT 25
#define DIST_APART 300
#define SPEED_LIMIT FVector(25.0f)

void UFlockingManager::Init(UWorld* world, UStaticMeshComponent* mesh) {
    UE_LOG(LogTemp, Warning, TEXT("Manager initialized"));

    World = world;
    float incr = (PI * 2.f) / AGENT_COUNT;
    for (int i = 0; i < AGENT_COUNT; i++) {
        if (World != nullptr) {
            FRotator rotation = FRotator();

            FVector location = FVector();
            location.Y = FMath::Sin(incr * i) * 150.f + FMath::RandRange(-1000, 1000);
            location.Z = FMath::Cos(incr * i) * 150.f + +FMath::RandRange(-1000, 1000);

            AAgent* agent = World->SpawnActor<AAgent>(location, rotation);
            agent->Init(mesh, i);
            Agents.Add(agent);
        }
    }

    initialized = true;
}

void UFlockingManager::Flock() {
    for (int i = 0; i < Agents.Num(); i++) {
        AAgent* agent = Agents[i];
        FVector v1, v2, v3, g;
        
        v1 = Rule1(agent);
        v2 = Rule2(agent);
        v3 = Rule3(agent);
        

        agent->Velocity += v1 + v2 + v3;
        Goal(agent);
    }
}

FVector UFlockingManager::Rule1(AAgent* b) {
    FVector pc = FVector();
    for (int i = 0; i < Agents.Num(); i++) {
        AAgent* agent = Agents[i];
        if (agent != b) {
            pc += agent->GetActorLocation();
        }
    }

    pc /= (Agents.Num() - 1);
    return (pc - b->GetActorLocation()) / 100;
}
FVector UFlockingManager::Rule2(AAgent* b) {
    FVector c = FVector(0.f);
    for (int i = 0; i < Agents.Num(); i++) {
        AAgent* agent = Agents[i];
        if (agent != b) {
            float dist = FVector::Dist(agent->GetActorLocation(), b->GetActorLocation());
            if (FMath::Abs(dist) < DIST_APART) {
                c -= (agent->GetActorLocation() - b->GetActorLocation());
            }
        }
    }

    return c;
}
FVector UFlockingManager::Rule3(AAgent* b) {
    FVector pv = FVector(0.f);
    for (int i = 0; i < Agents.Num(); i++) {
        AAgent* agent = Agents[i];
        if (agent != b) {
            pv += agent->Velocity;
        }
    }

    pv /= (Agents.Num() - 1);
    return (pv - b->Velocity) / 8;
}

void UFlockingManager::Goal(AAgent* b) {
    if(!use_goal) return;
    int limit = 25;
    if ((b->Velocity).GetAbs().Size() > limit) {
        b->Velocity = ((b->Velocity) / (b->Velocity).GetAbs().Size()) * limit;
    }
}

