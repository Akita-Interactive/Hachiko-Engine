pushd "%~dp0" 
set current_dir=%cd%
py.exe %current_dir%\run_code_generation.py
popd