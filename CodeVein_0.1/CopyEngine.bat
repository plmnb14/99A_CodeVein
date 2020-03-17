
xcopy /y .\Engine\Bin\*.dll				.\Client\Bin\
xcopy /y .\Engine\Bin\*.dll				.\x64\Debug\

xcopy /y .\Engine\Bin\*.dll				.\EffectTool\Codes\
xcopy /y .\Engine\Bin\*.dll				.\EffectTool\Codes\x64\Debug\
xcopy /y .\Engine\Bin\*.dll				.\EffectTool\Bin\

xcopy /y .\Engine\Bin\ShaderFiles\*.fx	.\Client\Bin\ShaderFiles\
xcopy /y .\Client\Bin\ShaderFiles\*.fx	.\Tool\Bin\ShaderFiles\
xcopy /y .\Client\Bin\ShaderFiles\*.fx	.\EffectTool\Bin\ShaderFiles\

xcopy /y .\Engine\Bin\*.lib				.\Reference\Librarys\
xcopy /y .\Engine\Headers\*.h			.\Reference\Headers\