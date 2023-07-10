#include <cstdlib>
#include <type_traits>
#include <iostream>
#include <string>

#include <QSGNode>

#include "QskLevelingSensorUtility.h"

#define ASSERT_TRUE(expr) if(!(expr)) { std::cerr << "ASSERT_TRUE(!" << #expr << "):" << __LINE__ << '\n'; return false; }

class QSGNodeBase : public QSGNode
{
public:
    virtual std::string name() const = 0;
};

class QSGNodeA final : public QSGNodeBase{ public: QSGNodeA() { std::cout << name() << '\n'; } std::string name() const override { return "A"; }};
class QSGNodeB final : public QSGNodeBase{ public: QSGNodeB() { std::cout << name() << '\n'; } std::string name() const override { return "B"; }};
class QSGNodeC final : public QSGNodeBase{ public: QSGNodeC() { std::cout << name() << '\n'; } std::string name() const override { return "C"; }};
class QSGNodeD final : public QSGNodeBase{ public: QSGNodeD() { std::cout << name() << '\n'; } std::string name() const override { return "D"; }};
class QSGNodeE final : public QSGNodeBase{ public: QSGNodeE() { std::cout << name() << '\n'; } std::string name() const override { return "E"; }};
class QSGNodeF final : public QSGNodeBase{ public: QSGNodeF() { std::cout << name() << '\n'; } std::string name() const override { return "F"; }};
class QSGNodeG final : public QSGNodeBase{ public: QSGNodeG() { std::cout << name() << '\n'; } std::string name() const override { return "G"; }};
class QSGNodeH final : public QSGNodeBase{ public: QSGNodeH() { std::cout << name() << '\n'; } std::string name() const override { return "H"; }};

using NodeType = QSGNode;

bool testcase_ensure_node()
{
    NodeType* root = nullptr;
    ASSERT_TRUE(root = ensure_node<NodeType>());
    ASSERT_TRUE(root->childCount() == 0);
    delete root;
    return true;
}

// seq<par<seq<seq<N<QSGNodeD, 4>>

/*

bool testcase_seq()
{
    using namespace qsg;
    {
        auto* const root = ensure_node<NodeType>();
        seq<N<QSGNodeA>, N<QSGNodeB>, N<QSGNodeC>>::append(root);
        delete root;
    }
    {
        auto* const root = ensure_node<NodeType>();
        ASSERT_TRUE(root);
        ASSERT_TRUE(root->childCount() == 0);
        qsg::seq<>::append(root);
        ASSERT_TRUE(root->childCount() == 0);
        delete root;
    }
    {
        auto* const root = ensure_node<NodeType>();
        ASSERT_TRUE(root);
        ASSERT_TRUE(root->childCount() == 0);
        qsg::seq<N<QSGNodeA>>::append(root);
        ASSERT_TRUE(root->childCount() == 1);
        delete root;
    }
    {
        auto* const root = ensure_node<NodeType>();
        ASSERT_TRUE(root);
        ASSERT_TRUE(root->childCount() == 0);
        qsg::seq<N<QSGNodeA>, N<QSGNodeB>>::append(root);
        ASSERT_TRUE(root->childCount() == 2);
        delete root;
    }
    {
        // - QSGNode
        //  - QSGNodeA
        //  - QSGNodeB
        //  - QSGNodeC
        auto* const root = ensure_node<NodeType>();
        ASSERT_TRUE(root);
        ASSERT_TRUE(root->childCount() == 0);
        qsg::seq<N<QSGNodeA>, N<QSGNodeB>, N<QSGNodeC>>::append(root);
        ASSERT_TRUE(root->childCount() == 3);
        delete root;
    }
    return true;
}

*/

bool testcase_par()
{
    using namespace qsg;
    {
        std::cout << __func__ << 0 << '\n';
        //auto* const root = ensure_node<QSGNode>();
        //ASSERT_TRUE(root->childCount() == 0);
        //par<4, QSGNodeA, par<4, QSGNodeB>>::append(root);
        //ASSERT_TRUE(root->childCount() == 4);
        //for(int i = 0; i < 4; ++i)
        //{
        //    ASSERT_TRUE(root->childAtIndex(i)->childCount() == 4);
        //}
        //delete root;
    }
    {
        std::cout << __func__ << 1 << '\n';
        // - QSGNode
        //  - QSGNodeA
        //   - QSGNodeB
        //   - QSGNodeC
        //   - QSGNodeD
        //  - QSGNodeA
        //   - QSGNodeB
        //   - QSGNodeC
        //   - QSGNodeD
        //  - QSGNodeA
        //   - QSGNodeB
        //   - QSGNodeC
        //   - QSGNodeD
        //  - QSGNodeA
        //   - QSGNodeB
        //   - QSGNodeC
        //   - QSGNodeD
        auto* const root = ensure_node<QSGNode>();
        ASSERT_TRUE(root->childCount() == 0);
        par<4, QSGNodeA, 
            seq<par<1, QSGNodeB>, 
                par<1, QSGNodeC>, 
                par<1, QSGNodeD>>>::append(root);
        ASSERT_TRUE(root->childCount() == 4);
        ASSERT_TRUE(root->childAtIndex(0)->childCount() == 3);
        ASSERT_TRUE(root->childAtIndex(1)->childCount() == 3);
        ASSERT_TRUE(root->childAtIndex(2)->childCount() == 3);
        ASSERT_TRUE(root->childAtIndex(3)->childCount() == 3);
    }
    {
        std::cout << __func__ << 3 << '\n';
        auto* const root = ensure_node<QSGNode>();
        seq<
            par<1, QSGNodeE, 
                seq<
                    par<1, QSGNodeA>, 
                    par<1, QSGNodeB>, 
                    par<1, QSGNodeC>>>,
            par<1, QSGNodeF, 
                seq<
                    par<1, QSGNodeB>, 
                    par<1, QSGNodeC>, 
                    par<1, QSGNodeA>>>,
            par<1, QSGNodeG, 
                seq<
                    par<1, QSGNodeC>, 
                    par<1, QSGNodeA>, 
                    par<1, QSGNodeB>>>
        >::append(root);
        ASSERT_TRUE(root->childCount() == 3);
        for(int i = 0; i < root->childCount(); ++i)
        {
            ASSERT_TRUE(root->childAtIndex(i)->childCount() == 3);
            for(int j = 0; j < root->childAtIndex(i)->childAtIndex(i)->childCount(); ++j)
            {
                ASSERT_TRUE(root->childAtIndex(i)->childCount() == 0);
            }
        }
        ASSERT_TRUE(dynamic_cast<QSGNodeE*>(root->childAtIndex(0)));
        ASSERT_TRUE(dynamic_cast<QSGNodeA*>(root->childAtIndex(0)->childAtIndex(0)));
        ASSERT_TRUE(dynamic_cast<QSGNodeB*>(root->childAtIndex(0)->childAtIndex(1)));
        ASSERT_TRUE(dynamic_cast<QSGNodeC*>(root->childAtIndex(0)->childAtIndex(2)));
        ASSERT_TRUE(dynamic_cast<QSGNodeF*>(root->childAtIndex(1)));
        ASSERT_TRUE(dynamic_cast<QSGNodeB*>(root->childAtIndex(1)->childAtIndex(0)));
        ASSERT_TRUE(dynamic_cast<QSGNodeC*>(root->childAtIndex(1)->childAtIndex(1)));
        ASSERT_TRUE(dynamic_cast<QSGNodeA*>(root->childAtIndex(1)->childAtIndex(2)));
        ASSERT_TRUE(dynamic_cast<QSGNodeG*>(root->childAtIndex(2)));
        ASSERT_TRUE(dynamic_cast<QSGNodeC*>(root->childAtIndex(2)->childAtIndex(0)));
        ASSERT_TRUE(dynamic_cast<QSGNodeA*>(root->childAtIndex(2)->childAtIndex(1)));
        ASSERT_TRUE(dynamic_cast<QSGNodeB*>(root->childAtIndex(2)->childAtIndex(2)));
    }
    return true;
}

int main()
{
    bool result = true;
    // result &= testcase_ensure_node();
    // result &= testcase_seq();
    result &= testcase_par();
    std::cout << (result ? "ok" : "failed") << std::endl;
    return result ? EXIT_SUCCESS : EXIT_FAILURE;
}