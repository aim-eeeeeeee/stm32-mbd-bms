function results = run_model_tests()
%RUN_MODEL_TESTS Execute the current BmsSupervisor smoke-test scenarios.
%   The saved harness remains unchanged. Its Constant sources are replaced in
%   memory with From Workspace sources and all model-reference outputs are
%   logged. Any failed expectation throws BmsSupervisor:ModelTestsFailed.

cfg = setup_model();
load_system(cfg.ModelFile);
load_system(cfg.HarnessFile);
cleanup = onCleanup(@()closeModels(cfg));

prepareHarness(cfg);
scenarios = defineScenarios();
results = repmat(struct( ...
    'Name', '', 'Passed', false, 'Message', ''), numel(scenarios), 1);

for scenarioIndex = 1:numel(scenarios)
    scenario = scenarios(scenarioIndex);
    results(scenarioIndex).Name = scenario.Name;
    try
        actual = simulateScenario(cfg, scenario);
        assertScenario(scenario, actual);
        results(scenarioIndex).Passed = true;
        results(scenarioIndex).Message = 'passed';
        fprintf('PASS: %s\n', scenario.Name);
    catch exception
        results(scenarioIndex).Message = exception.message;
        fprintf(2, 'FAIL: %s -- %s\n', scenario.Name, exception.message);
    end
end

failed = results(~[results.Passed]);
fprintf('\nBmsSupervisor: %u/%u scenarios passed.\n', ...
    nnz([results.Passed]), numel(results));
if ~isempty(failed)
    error('BmsSupervisor:ModelTestsFailed', ...
        '%u deterministic scenario(s) failed.', numel(failed));
end
end

function prepareHarness(cfg)
sourceNames = {'min_cell_mv', 'max_cell_mv', 'measurement_valid', ...
    'measurement_stale', 'step_ms'};
for sourceIndex = 1:numel(sourceNames)
    sourceName = sourceNames{sourceIndex};
    blockPath = [cfg.HarnessName '/' sourceName];
    replace_block(cfg.HarnessName, 'Name', sourceName, ...
        'simulink/Sources/From Workspace', 'noprompt');
    set_param(blockPath, ...
        'VariableName', [sourceName '_input'], ...
        'Interpolate', 'off', ...
        'OutputAfterFinalValue', 'Holding final value');
end

outputNames = {'state', 'warning_flags', 'fault_flags', ...
    'charge_allowed_shadow', 'discharge_allowed_shadow'};
ports = get_param([cfg.HarnessName '/Model'], 'PortHandles');
for outputIndex = 1:numel(outputNames)
    set_param(ports.Outport(outputIndex), ...
        'DataLogging', 'on', ...
        'DataLoggingNameMode', 'Custom', ...
        'DataLoggingName', outputNames{outputIndex});
end
set_param(cfg.HarnessName, ...
    'SignalLogging', 'on', ...
    'SignalLoggingName', 'logsout', ...
    'ReturnWorkspaceOutputs', 'on');
end

function scenarios = defineScenarios()
STATE_NORMAL = uint8(1);
STATE_SENSOR_ERROR = uint8(4);

scenarios = [ ...
    scenario('startup invalid and stale', 0, 0, false, true, ...
        STATE_SENSOR_ERROR, 0, 12, false, false), ...
    scenario('first valid normal sample', 3600, 3600, true, false, ...
        STATE_NORMAL, 0, 0, true, true), ...
    scenario('invalid to valid recovery', [3600 3600], [3600 3600], ...
        [false true], [false false], [STATE_SENSOR_ERROR STATE_NORMAL], ...
        [0 0], [4 0], [false true], [false true]), ...
    scenario('stale sample', 3600, 3600, true, true, ...
        STATE_SENSOR_ERROR, 0, 8, false, false) ...
    ];
end

function value = scenario(name, minMv, maxMv, valid, stale, state, ...
    warningFlags, faultFlags, chargeAllowed, dischargeAllowed)
value = struct( ...
    'Name', name, ...
    'MinCellMv', uint16(minMv), ...
    'MaxCellMv', uint16(maxMv), ...
    'Valid', logical(valid), ...
    'Stale', logical(stale), ...
    'State', uint8(state), ...
    'WarningFlags', uint16(warningFlags), ...
    'FaultFlags', uint16(faultFlags), ...
    'ChargeAllowed', logical(chargeAllowed), ...
    'DischargeAllowed', logical(dischargeAllowed));
end

function actual = simulateScenario(cfg, scenario)
sampleCount = max([numel(scenario.MinCellMv), numel(scenario.MaxCellMv), ...
    numel(scenario.Valid), numel(scenario.Stale)]);
time = (0:(sampleCount - 1))' * cfg.StepSeconds;

inputValues = { ...
    expandSignal(scenario.MinCellMv, sampleCount), ...
    expandSignal(scenario.MaxCellMv, sampleCount), ...
    expandSignal(scenario.Valid, sampleCount), ...
    expandSignal(scenario.Stale, sampleCount), ...
    repmat(cfg.StepMilliseconds, sampleCount, 1)};
variableNames = {'min_cell_mv_input', 'max_cell_mv_input', ...
    'measurement_valid_input', 'measurement_stale_input', 'step_ms_input'};

simulationInput = Simulink.SimulationInput(cfg.HarnessName);
for inputIndex = 1:numel(variableNames)
    inputSeries = timeseries(inputValues{inputIndex}, time);
    simulationInput = simulationInput.setVariable( ...
        variableNames{inputIndex}, inputSeries);
end
simulationInput = simulationInput.setModelParameter( ...
    'StopTime', sprintf('%.17g', time(end)));

simulationOutput = sim(simulationInput);
logs = simulationOutput.logsout;
actual.State = loggedData(logs, 'state');
actual.WarningFlags = loggedData(logs, 'warning_flags');
actual.FaultFlags = loggedData(logs, 'fault_flags');
actual.ChargeAllowed = loggedData(logs, 'charge_allowed_shadow');
actual.DischargeAllowed = loggedData(logs, 'discharge_allowed_shadow');
end

function values = expandSignal(values, sampleCount)
values = values(:);
if isscalar(values)
    values = repmat(values, sampleCount, 1);
elseif numel(values) ~= sampleCount
    error('BmsSupervisor:InvalidTestVector', ...
        'Every non-scalar input must have the scenario sample count.');
end
end

function values = loggedData(logs, signalName)
element = logs.getElement(signalName);
if isempty(element)
    error('BmsSupervisor:MissingLoggedSignal', ...
        'Harness did not log output signal %s.', signalName);
end
values = squeeze(element.Values.Data);
values = values(:).';
end

function assertScenario(expected, actual)
assertEqual(expected.Name, 'state', expected.State, actual.State);
assertEqual(expected.Name, 'warning_flags', ...
    expected.WarningFlags, actual.WarningFlags);
assertEqual(expected.Name, 'fault_flags', ...
    expected.FaultFlags, actual.FaultFlags);
assertEqual(expected.Name, 'charge_allowed_shadow', ...
    expected.ChargeAllowed, actual.ChargeAllowed);
assertEqual(expected.Name, 'discharge_allowed_shadow', ...
    expected.DischargeAllowed, actual.DischargeAllowed);
end

function assertEqual(scenarioName, signalName, expected, actual)
expected = expected(:).';
actual = cast(actual(:).', 'like', expected);
if ~isequal(actual, expected)
    error('BmsSupervisor:UnexpectedOutput', ...
        '%s: %s expected %s, received %s.', ...
        scenarioName, signalName, mat2str(expected), mat2str(actual));
end
end

function closeModels(cfg)
if bdIsLoaded(cfg.HarnessName)
    close_system(cfg.HarnessName, 0);
end
if bdIsLoaded(cfg.ModelName)
    close_system(cfg.ModelName, 0);
end
end
