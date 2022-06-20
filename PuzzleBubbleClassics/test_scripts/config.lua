openlib "utils"
openlib "base"

function importAllModels()
    currentDir = utils.getCurrentScriptDirectory()
    currentName = utils.files.filename(utils.getCurrentScriptPath())
    print(currentName)
    utils.files.forEachEntryInDirectory(currentDir, function(entry)
        if utils.files.isFile(entry) then
            name = utils.files.filename(entry)
            if name ~= currentName then
                import(entry)
            end
        end
    end)
end

importAllModels()

