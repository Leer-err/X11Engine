Vector3 = {}
Vector3.__index = Vector3

function Vector3:new(x, y, z)
    local t = setmetatable({}, self)

    t.x = x
    t.y = y
    t.z = z

    return t
end

function Vector3:__unm()
    return Vector3:new(-self.x, -self.y, -self.z)
end

function Vector3:__add(other)
    return Vector3:new(self.x + other.x, self.y + other.y, self.z + other.z)
end

function Vector3:__sub(other)
    return Vector3:new(self.x - other.x, self.y - other.y, self.z - other.z)
end

function Vector3:__mul(other)
    return Vector3:new(self.x * other, self.y * other, self.z * other)
end

function Vector3:__div(other)
    return Vector3:new(self.x / other, self.y / other, self.z / other)
end

function Vector3:length()
    return math.sqrt(self.x * self.x + self.y * self.y + self.z * self.z)
end

function Vector3:normalize(other)
    local len = self:length()
    return self / len
end

function Vector3:dot(other)
    return self.x * other.x + self.y * other.y + self.z * other.z
end

function Vector3:cross(other)
    local x = self.y * other.z - self.z * other.y
    local y = self.z * other.x - self.x * other.z
    local z = self.x * other.y - self.y * other.x

    return Vector3:new(x, y, z)
end
