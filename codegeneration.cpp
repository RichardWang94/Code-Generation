#include "codegeneration.hpp"

// CodeGenerator Visitor Functions: These are the functions
// you will complete to generate the x86 assembly code. Not
// all functions must have code, many may be left empty.

void CodeGenerator::visitProgramNode(ProgramNode* node) {
  std::cout<<"  .data"<<std::endl;
  std::cout<<"  printstr: .asciz \"%d\\n\""<<std::endl;
  std::cout<<"  .text"<<std::endl;
  std::cout<<"  .globl Main_main"<<std::endl;
  node->visit_children(this);
}

void CodeGenerator::visitClassNode(ClassNode* node) {
  currentClassName = node->identifier_1->name;
  std::map<std::string, ClassInfo>::iterator it;
  it = classTable->find(currentClassName);
  currentClassInfo = it->second;
  node->visit_children(this);
}

void CodeGenerator::visitMethodNode(MethodNode* node) {
  currentMethodName = node->identifier->name;
  std::map<std::string, MethodInfo>::iterator it;
  it = currentClassInfo.methods->find(currentMethodName);
  currentMethodInfo = it->second;
  std::cout<<"  "<< currentClassName << "_" << currentMethodName << ":" << std::endl;
  std::cout << "  push %ebp" << std::endl;
  std::cout << "  mov %esp, %ebp" << std::endl;
  std::cout << "  sub " << "$" << currentMethodInfo.localsSize << ", %esp" << std::endl;
  node->visit_children(this);
  std::cout << "  mov %ebp, %esp" << std::endl;
  std::cout << "  pop %ebp" << std::endl;
  std::cout << "  ret" << std::endl;
}

void CodeGenerator::visitMethodBodyNode(MethodBodyNode* node) {
  node->visit_children(this);
}

void CodeGenerator::visitParameterNode(ParameterNode* node) {
  node->visit_children(this);
}

void CodeGenerator::visitDeclarationNode(DeclarationNode* node) {
  node->visit_children(this); 
}

void CodeGenerator::visitReturnStatementNode(ReturnStatementNode* node) {
  node->visit_children(this);
  std::cout << "  pop %eax" << std::endl;
}

void CodeGenerator::visitAssignmentNode(AssignmentNode* node) {  
  node->visit_children(this);
  std::cout << "  #Assignment" << std::endl;
  std::string nodeName = node->identifier_1->name;
  if (node->identifier_2 == NULL) {
    if (currentClassInfo.members->find(nodeName) != currentClassInfo.members->end()) {
      std::cout << "  pop %eax" << std::endl;
      std::cout << "  mov 8(%ebp), %ebx" << std::endl;
      std::cout << "  mov %eax," << currentClassInfo.members->find(nodeName)->second.offset << "(%ebx)" << std::endl;
    }
  else
    {
      std::cout << "  pop %eax" << std::endl;
      std::cout << "  mov %eax," << currentMethodInfo.variables->find(nodeName)->second.offset << "(%ebp)" << std::endl;
    }
  }
  else {
       std::string id1 = node->identifier_1->name;
    std::string memberName = node->identifier_2->name;
    int size;
    if (currentClassInfo.members->find(id1) != currentClassInfo.members->end()) {
      VariableInfo objinfo = currentClassInfo.members->find(id1)->second;
      std::cout << "  pop %eax" << std::endl;
      std::cout << "  mov 8(%ebp), %ebx" << std::endl;
      std::cout << "  mov %eax," << classTable->find(objinfo.type.objectClassName)->second.members->find(memberName)->second.offset << "(%ebx)" << std::endl;
    }
    else
      {
	VariableInfo objinfo = currentMethodInfo.variables->find(id1)->second;
	std::cout << "  pop %eax" << std::endl;
	
	//std::cout << "  push " << classTable->find(objinfo.type.objectClassName)->second.members->find(memberName)->second.offset << "(%ebp)" <<  std::endl;
	//std::cout<< "  #Print"<<std::endl;
	//std::cout<< "  push $printstr" << std::endl;
	//std::cout<<"  call printf" << std::endl;
	//std::cout<<"  add $8, %esp" << std::endl;

  	//std::cout << "  push %ebp" << std::endl;
	  //std::cout<< "  #Print"<<std::endl;
	  //std::cout<< "  push $printstr" << std::endl;
	  //std::cout<<"  call printf" << std::endl;
	  //std::cout<<"  add $8, %esp" << std::endl;
  
	  std::cout << "  mov " << objinfo.offset << "(%ebp), %ebx" << std::endl;
	 // std::cout << "  mov %eax," << classTable->find(objinfo.type.objectClassName)->second.members->find(memberName)->second.offset << " (%ebp
	std::cout << "  mov %eax," << classTable->find(objinfo.type.objectClassName)->second.members->find(memberName)->second.offset << "(%ebx)" << std::endl;
	} 
  }
}

void CodeGenerator::visitCallNode(CallNode* node) {
  node->visit_children(this);
  std::cout << "  add $4, %esp" << std::endl;
}

void CodeGenerator::visitIfElseNode(IfElseNode* node) {
  int ifElseLabel = nextLabel();
  node->expression->accept(this);
  std::cout << "  # ifElse" << std::endl;
  std::cout << "  pop %eax" << std::endl;
  std::cout << "  cmp $0, %eax" << std::endl;
  std::cout << "  je ELSE_"<< ifElseLabel << std::endl;
    for (std::list<StatementNode*>::iterator iter = node->statement_list_1->begin(); iter != node->statement_list_1->end(); iter++) {
    (*iter)->accept(this);
  }
  std::cout << "  jmp endIfELSE_" << ifElseLabel << std::endl;
  std::cout << "  ELSE_" << ifElseLabel << ":" << std::endl;
  if (node->statement_list_2 != NULL) {
        for (std::list<StatementNode*>::iterator iter = node->statement_list_2->begin(); iter != node->statement_list_2->end(); iter++) {
    (*iter)->accept(this);
    }
  }
  std::cout << "  endIfELSE_" << ifElseLabel << ":" << std::endl;
}

void CodeGenerator::visitWhileNode(WhileNode* node) {
  int whileLabel = nextLabel();
  std::cout << "  # while" << std::endl;
  std::cout << "  WHILE_" << whileLabel << ":" << std::endl;
  node->expression->accept(this);
  std::cout << "  pop %eax" << std::endl;
  std::cout << "  cmp $0, %eax" << std::endl;
  std::cout << "  je endWhile_"<< whileLabel << std::endl;
  for (std::list<StatementNode*>::iterator iter = node->statement_list->begin(); iter != node->statement_list->end(); iter++) {
    (*iter)->accept(this);
  }
  std::cout << "  jmp WHILE_" << whileLabel << std::endl;
  std::cout << "  endWhile_" << whileLabel << ":" << std::endl;
}

void CodeGenerator::visitRepeatNode(RepeatNode* node) {
  int repeatLabel = nextLabel();
  std::cout << "  # repeat" << std::endl;
  std::cout << "  REPEAT_" << repeatLabel << ":" << std::endl;
  for (std::list<StatementNode*>::iterator iter = node->statement_list->begin(); iter != node->statement_list->end(); iter++) {
    (*iter)->accept(this);
  }
  node->expression->accept(this);
  std::cout << "  pop %eax" << std::endl;
  std::cout << "  cmp $0, %eax" << std::endl;
  std::cout << "  je endRepeat_"<< repeatLabel << std::endl;
  std::cout << "  jmp REPEAT_" << repeatLabel << std::endl;
  std::cout << "  endRepeat_" << repeatLabel << ":" << std::endl; 
}

void CodeGenerator::visitPrintNode(PrintNode* node) {
  node->visit_children(this);
  std::cout<< "  #Print"<<std::endl;
  std::cout<< "  push $printstr" << std::endl;
  std::cout<<"  call printf" << std::endl;
  std::cout<<"  add $8, %esp" << std::endl;
}

void CodeGenerator::visitPlusNode(PlusNode* node) {
  node->visit_children(this);
  std::cout << "  # Plus" << std::endl;
  std::cout << "  pop %ebx" << std::endl;
  std::cout << "  pop %eax" << std::endl;
  std::cout << "  add %ebx, %eax" << std::endl;
  std::cout << "  push %eax" << std::endl;
}

void CodeGenerator::visitMinusNode(MinusNode* node) {
  node->visit_children(this);
  std::cout << "  # Minus" << std::endl;
  std::cout << "  pop %ebx" << std::endl;
  std::cout << "  pop %eax" << std::endl;
  std::cout << "  sub %ebx, %eax" << std::endl;
  std::cout << "  push %eax" << std::endl;
}

void CodeGenerator::visitTimesNode(TimesNode* node) {
  node->visit_children(this);
  std::cout << "  # Times" << std::endl;
  std::cout << "  pop %ebx" << std::endl;
  std::cout << "  pop %eax" << std::endl;
  std::cout << "  imul %ebx, %eax" << std::endl;
  std::cout << "  push %eax" << std::endl;  
}

void CodeGenerator::visitDivideNode(DivideNode* node) {
  node->visit_children(this);
  std::cout << "  # Divide" << std::endl;
  std::cout << "  pop %ebx" << std::endl;
  std::cout << "  pop %eax" << std::endl;
  std::cout << "  cdq" << std::endl;
  std::cout << "  idiv %ebx" << std::endl;
  std::cout << "  push %eax" << std::endl;    
}

void CodeGenerator::visitLessNode(LessNode* node) {
  node->visit_children(this);
  int ifLessLabel = nextLabel();
  std::cout << "  #Less" << std::endl;
  std::cout << "  pop %eax" << std::endl;
  std::cout << "  pop %ebx" << std::endl;
  std::cout << "  cmp %eax, %ebx" << std::endl;
  std::cout << "  jl ifLess_" << ifLessLabel << std::endl;
  std::cout << "  push $0" << std::endl;
  std::cout << "  jmp endLessThan_" << ifLessLabel<<std::endl;
  std::cout << "  ifLess_" << ifLessLabel << ":" << std::endl;
  std::cout << "  push $1" << std::endl;
  std::cout << "  endLessThan_" << ifLessLabel << ":" << std::endl;
}

void CodeGenerator::visitLessEqualNode(LessEqualNode* node) {
  node->visit_children(this);
  int ifLessEqualLabel = nextLabel();
  std::cout << "  #LessEqual" << std::endl;
  std::cout << "  pop %eax" << std::endl;
  std::cout << "  pop %ebx" << std::endl;
  std::cout << "  cmp %eax, %ebx" << std::endl;
  std::cout << "  jle ifLessEql_" << ifLessEqualLabel << std::endl;
  std::cout << "  push $0" << std::endl;
  std::cout << "  jmp endLessEql_" << ifLessEqualLabel<<std::endl;
  std::cout << "  ifLessEql_" << ifLessEqualLabel << ":" << std::endl;
  std::cout << "  push $1" << std::endl;
  std::cout << "  endLessEql_" << ifLessEqualLabel << ":" << std::endl;
}

void CodeGenerator::visitEqualNode(EqualNode* node) {
  node->visit_children(this);
  int EqualLabel = nextLabel();
  std::cout << "  #Equal" << std::endl;
  std::cout << "  pop %eax" << std::endl;
  std::cout << "  pop %ebx" << std::endl;
  std::cout << "  cmp %eax, %ebx" << std::endl;
  std::cout << "  je Equal_" << EqualLabel << std::endl;
  std::cout << "  push $0" << std::endl;
  std::cout << "  jmp endEql_" << EqualLabel<<std::endl;
  std::cout << "  Equal_" << EqualLabel << ":" << std::endl;
  std::cout << "  push $1" << std::endl;
  std::cout << "  endEql_" << EqualLabel << ":" << std::endl;
}

void CodeGenerator::visitAndNode(AndNode* node) {
  node->visit_children(this);
  std::cout << "  #And" << std::endl;
  std::cout << "  pop %eax" << std::endl;
  std::cout << "  pop %ebx" << std::endl;
  std::cout << "  and %eax, %ebx" << std::endl;
  std::cout << "  push %ebx" << std::endl;
}

void CodeGenerator::visitOrNode(OrNode* node) {
  node->visit_children(this);
  std::cout << "  #Or" << std::endl;
  std::cout << "  pop %eax" << std::endl;
  std::cout << "  pop %ebx" << std::endl;
  std::cout << "  or %eax, %ebx" << std::endl;
  std::cout << "  push %ebx" << std::endl;
}

void CodeGenerator::visitNotNode(NotNode* node) {
  node->visit_children(this);
  std::cout << "  #Not" << std::endl;
  std::cout << "  pop %eax" << std::endl;
  std::cout << "  pop %eax" << std::endl;
  std::cout << "  push %eax" << std::endl;
}

void CodeGenerator::visitNegationNode(NegationNode* node) {
  node->visit_children(this);
  std::cout << "  #Negation" << std::endl;
  std::cout << "  pop %eax" << std::endl;
  std::cout << "  neg %eax" << std::endl;
  std::cout << "  push %eax" << std::endl;
}

void CodeGenerator::visitMethodCallNode(MethodCallNode* node) {
  int size = 4;
  std::string className = "";
  std::cout << "  # Method Call" << std::endl;
  std::cout << "  push %ecx" << std::endl;
  std::cout << "  push %edx" << std::endl;
  for (std::list<ExpressionNode*>::reverse_iterator iter = node->expression_list->rbegin(); iter != node->expression_list->rend(); iter++) {
    (*iter)->accept(this);
    size += 4;
  }
  if (node->identifier_2 != NULL) {
    std::string memberName = node->identifier_1->name;
    std::string methodName = node->identifier_2->name;
    std::string className = "";
    std::string className1 = "";
    std::string className2 = "";
    if (currentClassInfo.members->find(memberName) != currentClassInfo.members->end()) {
      std::cout << "  mov 8(%ebp), %ebx" << std::endl;
      std::cout << "  push " << currentClassInfo.members->find(memberName)->second.offset << "(%ebx)" << std::endl;
    }
    else
      {
	std::cout << "  push " << currentMethodInfo.variables->find(memberName)->second.offset << "(%ebp)" << std::endl;
      }
    for (std::map<std::string, ClassInfo>::iterator it = classTable->begin(); it != classTable->end(); it++) {
      if (it->second.methods->find(methodName) != it->second.methods->end())
	className = it->first;
    }
    std::cout << "  call " << className << "_" << methodName << std::endl;
    std::cout << "  add $" << size << ", %esp" << std::endl;
    std::cout << "  pop %edx" << std::endl;
    std::cout << "  pop %ecx" << std::endl;
    std::cout << "  push %eax" << std::endl;
  }
  else
    {
      std::cout << "  push 8(%ebp)" << std::endl;
      std::string methodName = node->identifier_1->name;
      std::cout << "  call " << currentClassName << "_" << methodName << std::endl;
      std::cout << "  add $" << size << ", %esp"<< std::endl;
      std::cout << "  pop %edx" << std::endl;
      std::cout << "  pop %ecx" << std::endl;
      std::cout << "  push %eax" << std::endl;
    }
}

void CodeGenerator::visitMemberAccessNode(MemberAccessNode* node) {
  node->visit_children(this);
  std::cout << " #Member Access" << std::endl;
  std::string id1 = node->identifier_1->name;
  std::string memberName = node->identifier_2->name;
  int size;
  if (currentClassInfo.members->find(id1) != currentClassInfo.members->end()) {
    VariableInfo objinfo = currentClassInfo.members->find(id1)->second;
    std::cout << "  mov 8(%ebp), %ebx" << std::endl;
    std::cout << "  mov " << objinfo.offset << "(%ebx)" << ",%ebx" << std::endl;
    std::cout << "  push " << classTable->find(objinfo.type.objectClassName)->second.members->find(memberName)->second.offset << "(%ebx)" << std::endl;
  }
  else
    {
           VariableInfo objinfo = currentMethodInfo.variables->find(id1)->second;
      std::cout << "  mov " << objinfo.offset << "(%ebp), %eax" << std::endl;
      // std::cout << "  push " << classTable->find(objinfo.type.objectClassName)->second.members->find(memberName)->second.offset << "(%ebx)" << std::endl;
      std::cout << "  push " << classTable->find(objinfo.type.objectClassName)->second.members->find(memberName)->second.offset << "(%eax)" << std::endl; 


      //std::cout << "  push " << classTable->find(objinfo.type.objectClassName)->second.members->find(memberName)->second.offset << "(%ebp)" << std::endl;
      //std::cout<< "  #Print"<<std::endl;
      // std::cout<< "  push $printstr" << std::endl;
      //std::cout<<"  call printf" << std::endl;
      //std::cout<<"  add $8, %esp" << std::endl;

      //	std::cout << "  push %ebp" << std::endl;
      //std::cout<< "  #Print"<<std::endl;
      //std::cout<< "  push $printstr" << std::endl;
      //std::cout<<"  call printf" << std::endl;
      //std::cout<<"  add $8, %esp" << std::endl;
    }
}

void CodeGenerator::visitVariableNode(VariableNode* node) {
  node->visit_children(this);
  std::string Var_Name = node->identifier->name;
  std::cout << "  #Variable " << std::endl;
  if (currentClassInfo.members->find(Var_Name) != currentClassInfo.members->end())
    {
      std::cout << "  mov 8(%ebp), %ebx" << std::endl;
      std::cout << "  push " << currentClassInfo.members->find(Var_Name)->second.offset << "(%ebp)" << std::endl;
    }
  else std::cout << "  push " << currentMethodInfo.variables->find(Var_Name)->second.offset << "(%ebp)" << std::endl;
}

void CodeGenerator::visitIntegerLiteralNode(IntegerLiteralNode* node) {  
  node->visit_children(this);
  std::cout << "  # Integer Literal" << std::endl;
  std::cout << "  push $" << node->integer->value << std::endl;
  
}

void CodeGenerator::visitBooleanLiteralNode(BooleanLiteralNode* node) {
  node->visit_children(this);
  std::cout << "  # Boolean Literal" << std::endl;
  std::cout << "  push $" << node->integer->value << std::endl;
}

void CodeGenerator::visitNewNode(NewNode* node) {
  std::cout << "  # New" << std::endl;
  int size = classTable->find(node->identifier->name)->second.membersSize;
  std::cout << "  push $" << size << std::endl;
  std::cout << "  call malloc" << std::endl;
  std::cout << "  add $4, %esp" << std::endl;
  std::cout << "  push %eax" << std::endl;
}

void CodeGenerator::visitIntegerTypeNode(IntegerTypeNode* node) {
  node->visit_children(this);
}

void CodeGenerator::visitBooleanTypeNode(BooleanTypeNode* node) {
  node->visit_children(this);
}

void CodeGenerator::visitObjectTypeNode(ObjectTypeNode* node) {
  node->visit_children(this);
}

void CodeGenerator::visitNoneNode(NoneNode* node) {
  node->visit_children(this);
}

void CodeGenerator::visitIdentifierNode(IdentifierNode* node) {
   node->visit_children(this);
}

void CodeGenerator::visitIntegerNode(IntegerNode* node) {
   node->visit_children(this);
}
