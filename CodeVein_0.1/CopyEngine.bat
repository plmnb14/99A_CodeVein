
xcopy /y .\Engine\Bin\*.dll				.\Client\Bin\
xcopy /y .\Engine\Release\*.dll			.\Client\Release\
xcopy /y .\Engine\Bin\*.dll				.\x64\Debug\
xcopy /y .\Engine\Release\*.dll			.\x64\Release\

xcopy /y .\Engine\Bin\*.dll				.\EffectTool\Codes\
xcopy /y .\Engine\Bin\*.dll				.\EffectTool\Codes\x64\Debug\
xcopy /y .\Engine\Bin\*.dll				.\EffectTool\Bin\

xcopy /y .\Engine\Bin\ShaderFiles\*.fx	.\Client\ShaderFiles\
xcopy /y .\Client\ShaderFiles\*.fx		.\Tool\ShaderFiles\
xcopy /y .\Client\ShaderFiles\*.fx		.\EffectTool\ShaderFiles\

xcopy /y .\Engine\Bin\*.lib				.\Reference\Librarys\
xcopy /y .\Engine\Headers\*.h			.\Reference\Headers\