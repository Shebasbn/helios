///////////////////////////////////////////////////////
// NOTE(Sebas): Platform Input Functions

function B32 
platform_push_event(PlatformEventQueue* queue, PlatformEvent event)
{
    LightAssert(queue->count < MAX_PLATFORM_EVENTS);
    B32 result = false;
    if (queue->count < MAX_PLATFORM_EVENTS)
    {
        queue->event[queue->count] = event;
        queue->count++;
        result = true;
    }
    return true;
}

function B32
platform_pop_event(PlatformEventQueue* queue, PlatformEvent* out_event)
{
    B32 result = 0;
    if (queue->count > 0)
    {

    }
    return result;
}