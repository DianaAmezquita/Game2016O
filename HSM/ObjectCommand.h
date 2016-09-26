#pragma once
class CObjectBase;

/*
   Las aplicaciones pueden encapsular secuencias de operaciones sobre cualquier objeto de la clase CObjectBase a través de 
   clases derivadas de CObjectCommand.

   Para definir cualquier secuencia de operaciones, deberá derivar sus comandos a partir de la clase CObjectCommand.

   Ejemplos : CCmdLoadResources,  CCmdApplyTransformations pueden especificar una estrategia o secuencia de acciones sobre un objeto.

   Para aplicar comandos de forma masiva o específica, véase la clase CObjectStock y los métodos CObjectStock::ForEach y CObjectStock::ForEachInSet

   Los comandos derivados, deberán implementar el método DoCommand quien ejecuta la secuencia de comandos específica sobre el objeto. 
   Nota: La ejecución no se limita a acciones aplicables sobre el objeto, si agrega variables miembro a su nuevo comando que deriva de CObjectCommand, 
   puede establecer parámetros que controlan la ejecución de su implementación DoCommand. Las posibilidades son incontables.
*/

class CObjectCommand
{
public:
	//Estrategia de ejecución (comando) sobre el objeto.
	virtual bool DoCommand(CObjectBase* pTarget)=0;
	CObjectCommand(void);
	~CObjectCommand(void);
};

