#pragma once

template <typename TState>
class GECodable {
public:
    virtual ~GECodable() = default;
    virtual TState snapshotState() const = 0;
    virtual void applyState(const TState& state) = 0;
};