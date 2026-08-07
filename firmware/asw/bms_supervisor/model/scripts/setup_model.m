function cfg = setup_model()
%SETUP_MODEL Configure paths and generated-file locations for BmsSupervisor.
%   CFG = SETUP_MODEL() derives all paths from this file, verifies required
%   products and source files, adds only model-development folders to the
%   MATLAB path, and redirects MATLAB output under the repository's visible
%   top-level codegen/ workspace.

scriptFile = mfilename('fullpath');
if isempty(scriptFile)
    error('BmsSupervisor:SetupLocationUnknown', ...
        'setup_model.m must be run from its saved repository location.');
end

cfg.ScriptsDir = fileparts(scriptFile);
cfg.ModelRoot = fileparts(cfg.ScriptsDir);
cfg.ComponentRoot = fileparts(cfg.ModelRoot);
cfg.FirmwareRoot = fileparts(fileparts(cfg.ComponentRoot));
cfg.RepoRoot = fileparts(cfg.FirmwareRoot);
cfg.TestsDir = fullfile(cfg.ComponentRoot, 'tests');
cfg.ModelName = 'BmsSupervisor';
cfg.HarnessName = 'BmsSupervisor_Harness';
cfg.ModelFile = fullfile(cfg.ModelRoot, [cfg.ModelName '.slx']);
cfg.HarnessFile = fullfile(cfg.ModelRoot, [cfg.HarnessName '.slx']);
cfg.CodeGenRoot = fullfile(cfg.RepoRoot, 'codegen');
cfg.CacheFolder = fullfile(cfg.CodeGenRoot, 'cache');
cfg.CodeGenFolder = cfg.CodeGenRoot;
cfg.StageFolder = fullfile(cfg.ComponentRoot, 'src', 'generated');
cfg.StepSeconds = 0.5;
cfg.StepMilliseconds = uint16(500);

requiredFiles = {cfg.ModelFile, cfg.HarnessFile};
for fileIndex = 1:numel(requiredFiles)
    if ~isfile(requiredFiles{fileIndex})
        error('BmsSupervisor:MissingFile', ...
            'Required model file does not exist: %s', requiredFiles{fileIndex});
    end
end

requiredProducts = {'Simulink', 'Stateflow', 'Simulink Coder'};
installedProducts = ver;
installedNames = {installedProducts.Name};
for productIndex = 1:numel(requiredProducts)
    productName = requiredProducts{productIndex};
    if ~any(strcmp(installedNames, productName))
        error('BmsSupervisor:MissingProduct', ...
            'Required MathWorks product is unavailable: %s', productName);
    end
end

embeddedCoderTarget = fullfile(matlabroot, 'rtw', 'c', 'ert', 'ert.tlc');
if ~license('test', 'RTW_Embedded_Coder') || ~isfile(embeddedCoderTarget)
    error('BmsSupervisor:MissingProduct', ...
        'Embedded Coder and its ert.tlc target are required.');
end

foldersToCreate = {cfg.CodeGenRoot, cfg.CacheFolder};
for folderIndex = 1:numel(foldersToCreate)
    if ~isfolder(foldersToCreate{folderIndex})
        mkdir(foldersToCreate{folderIndex});
    end
end

addpath(cfg.ModelRoot, cfg.ScriptsDir, cfg.TestsDir);
Simulink.fileGenControl('set', ...
    'CacheFolder', cfg.CacheFolder, ...
    'CodeGenFolder', cfg.CodeGenFolder, ...
    'CodeGenFolderStructure', ...
        Simulink.filegen.CodeGenFolderStructure.ModelSpecific, ...
    'keepPreviousPath', false, ...
    'createDir', true);

fprintf('BmsSupervisor environment ready.\n');
fprintf('  Model:    %s\n', cfg.ModelFile);
fprintf('  Harness:  %s\n', cfg.HarnessFile);
fprintf('  Cache:    %s\n', cfg.CacheFolder);
fprintf('  Codegen:  %s\n', cfg.CodeGenFolder);
fprintf('  Staging:  %s\n', cfg.StageFolder);
end
