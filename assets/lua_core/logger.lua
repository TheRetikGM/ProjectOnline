LogType = {
    info = 0,
    status = 1,
    warning = 2,
    error = 3,
    critical = 4
}

local function File()
    return debug.getinfo(3, 'S').source
end

local function Line()
    return debug.getinfo(3, 'l').currentline
end

-- Emit info log message
function LogI(message)
    API_Log(LogType.info, "" .. message, File(), Line())
end

-- Emit status log message
function LogS(message)
    API_Log(LogType.status, "" .. message, File(), Line())
end

-- Emit warning log message
function LogW(message)
    API_Log(LogType.warning, "" .. message, File(), Line())
end

-- Emit error log message
function LogE(message)
    API_Log(LogType.error, "" .. message, File(), Line())
end

-- Emit critical log message
function LogC(message)
    API_Log(LogType.critical, "" .. message, File(), Line())
end
