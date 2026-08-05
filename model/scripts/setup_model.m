%SETUP_MODEL Configure the reproducible BmsSupervisor modeling environment.
% This script derives every project path from its own location, verifies the
% required MathWorks products, and defines shared model setup data.

scriptFile = mfilename('fullpath');
if isempty(scriptFile)
    error('BmsSupervisor:SetupLocationUnknown', ...
        'Run setup_model.m as a saved file so its location can be determined.');
end

scriptsDir = fileparts(scriptFile);
modelRoot = fileparts(scriptsDir);
repoRoot = fileparts(modelRoot);
dataDir = fullfile(modelRoot, 'data');
testsDir = fullfile(modelRoot, 'tests');

requiredFolders = {modelRoot, dataDir, scriptsDir, testsDir};
for folderIndex = 1:numel(requiredFolders)
    if ~isfolder(requiredFolders{folderIndex})
        error('BmsSupervisor:MissingFolder', ...
            'Required model folder does not exist: %s', ...
            requiredFolders{folderIndex});
    end
end

addpath(modelRoot, dataDir, scriptsDir, testsDir);

installedProducts = ver;
installedNames = {installedProducts.Name};
requiredProducts = { ...
    'Simulink', ...
    'Stateflow', ...
    'Simulink Coder'};

for productIndex = 1:numel(requiredProducts)
    productName = requiredProducts{productIndex};
    if ~any(strcmp(installedNames, productName))
        error('BmsSupervisor:MissingProduct', ...
            'Required MathWorks product is unavailable: %s', productName);
    end
end

% Embedded Coder can be licensed and installed even when it is not returned
% as a separate entry by ver. Verify both its license and its ERT target file.
embeddedCoderTarget = fullfile(matlabroot, 'rtw', 'c', 'ert', 'ert.tlc');
if ~license('test', 'RTW_Embedded_Coder') || ~isfile(embeddedCoderTarget)
    error('BmsSupervisor:MissingProduct', ...
        ['Embedded Coder is unavailable. The RTW_Embedded_Coder license ', ...
         'and ERT system target file are both required.']);
end

% Frozen MVP model period from BMS_SUPERVISOR_REQUIREMENTS.md.
BMS_MODEL_STEP_MS = uint16(500);

fprintf('BmsSupervisor model environment ready.\n');
fprintf('  MATLAB release: %s\n', version('-release'));
fprintf('  Repository root: %s\n', repoRoot);
fprintf('  Model root: %s\n', modelRoot);
fprintf('  Model step: %u ms\n', BMS_MODEL_STEP_MS);
