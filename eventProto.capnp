@0x86067e6a0fae5e5e;

struct FullData{
    type @0: UInt8;
    events @1: List(FullEvent);
}

struct PlainData{
    type @0: UInt8;
    events @1: List(PlainEvent);
}

struct PsdData{
    type @0: UInt8;
    events @1: List(PsdEvent);
}

struct WaveData{
    type @0: UInt8;
    events @1: List(WaveEvent);
}

struct DualWaveData{
    type @0: UInt8;
    events @1: List(DualWaveEvent);
}

struct RawTimeData{
    type @0: UInt8;
    events @1: List(RawTimeEvent);
}

struct CrossData{
    type @0: UInt8;
    events @1: List(CrossEvent);
}

struct PsdWaveData{
    type @0: UInt8;
    events @1: List(PsdWaveEvent);
}

struct FullEvent {
    board @0 :UInt8;
    channel @1 :UInt8;
    energy @2: UInt16;
    timestamp @3: UInt64;
    psd @4: Float32;
    waveform1 @5: List(Int16);
    waveform2 @6: List(Int16);
}

struct PlainEvent {
    board @0 :UInt8;
    channel @1 :UInt8;
    energy @2: UInt16;
    timestamp @3: UInt64; 
}

struct PsdEvent {
    board @0 :UInt8;
    channel @1 :UInt8;
    energy @2: UInt16;
    timestamp @3: UInt64;
    psd @4: Float32;
}

struct WaveEvent {
    board @0 :UInt8;
    channel @1 :UInt8;
    energy @2: UInt16;
    timestamp @3: UInt64;
    waveform1 @4: List(Int16);   
}

struct DualWaveEvent {
    board @0 :UInt8;
    channel @1 :UInt8;
    energy @2: UInt16;
    timestamp @3: UInt64;
    waveform1 @4: List(Int16);
    waveform2 @5: List(Int16);    
}

struct RawTimeEvent {
    board @0 :UInt8;
    channel @1 :UInt8;
    energy @2: UInt16;
    timestamp @3: UInt64;
    fineTimestamp @4: UInt16;
}

struct CrossEvent{
    board @0 :UInt8;
    channel @1 :UInt8;
    energy @2: UInt16; 
    timestamp @3: UInt64;
    goodTrigger @4: Bool;
    lostTrigger @5: Bool;
}

struct PsdWaveEvent {
    board @0 :UInt8;
    channel @1 :UInt8;
    energy @2: UInt16;
    timestamp @3: UInt64;
    psd @4: Float32;
    waveform1 @5: List(Int16);
}

struct Dataset {
    events @0: List(Event);

    struct Event {
        board @0 :UInt8;
        channel @1 :UInt8;
        energy @2: UInt16;
        timestamp @3: UInt64;
        psd @4: Float32;
        waveform @5: List(Int16);
    }
}