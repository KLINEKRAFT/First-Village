#include "CivHistorySubsystem.h"

void UCivHistorySubsystem::RecordEvent(const FCivHistoryEvent& Event)
{
    Events.Add(Event);
    constexpr int32 MaxEvents = 5000;
    if (Events.Num() > MaxEvents)
    {
        Events.RemoveAt(0, Events.Num() - MaxEvents);
    }
}

TArray<FCivHistoryEvent> UCivHistorySubsystem::GetRecentEvents(int32 MaxCount) const
{
    TArray<FCivHistoryEvent> Result;
    const int32 Count = FMath::Clamp(MaxCount, 0, Events.Num());
    const int32 Start = Events.Num() - Count;
    for (int32 i = Start; i < Events.Num(); ++i)
    {
        Result.Add(Events[i]);
    }
    return Result;
}
