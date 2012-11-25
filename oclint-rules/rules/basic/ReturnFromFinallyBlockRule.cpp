#include <clang/AST/AST.h>

#include "oclint/Rule.h"
#include "oclint/RuleSet.h"


class ReturnFromFinallyBlockRule : public Rule<ReturnFromFinallyBlockRule>
{
    class ExtractReturnStmts : public RecursiveASTVisitor<ExtractReturnStmts>
    {
    private:
        vector<ReturnStmt*> *_returns;

    public:
        void extract(CompoundStmt *finallyBlock, vector<ReturnStmt*> *returns)
        {
            _returns = returns;
            TraverseStmt(finallyBlock);
        }

        bool VisitReturnStmt(ReturnStmt *returnStmt)
        {
            _returns->push_back(returnStmt);
            return true;
        }
    };

private:
    static RuleSet rules;

public:
    virtual const string name() const
    {
        return "return from finally block";
    }

    virtual const int priority() const
    {
        return 2;
    }

    bool VisitObjCAtFinallyStmt(ObjCAtFinallyStmt *finallyStmt)
    {
        CompoundStmt *compoundStmt = dyn_cast<CompoundStmt>(finallyStmt->getFinallyBody());
        if (compoundStmt)
        {
            vector<ReturnStmt*> *returns = new vector<ReturnStmt*>();
            ExtractReturnStmts extractReturnStmts;
            extractReturnStmts.extract(compoundStmt, returns);
            for (int index = 0; index < returns->size(); index++)
            {
                ReturnStmt *returnStmt = returns->at(index);
                addViolation(returnStmt, this);
            }
        }

        return true;
    }
};

RuleSet ReturnFromFinallyBlockRule::rules(new ReturnFromFinallyBlockRule());
