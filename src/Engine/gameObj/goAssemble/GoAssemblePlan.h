/*
 * ===================== GoAssemblePlan.h =====================
 *                          -- tpr --
 *                                        CREATE -- 2019.10.10
 *                                        MODIFY -- 
 * ----------------------------------------------------------
 */
#ifndef TPR_GO_ASSEMBLE_PLAN_H
#define TPR_GO_ASSEMBLE_PLAN_H
//--- glm - 0.9.9.5 ---
#include "glm_no_warnings.h"

//------------------- CPP --------------------//
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional> // hash
#include <optional>
#include <variant>

//------------------- Engine --------------------//
#include "tprAssert.h"
#include "GameObjType.h"
#include "AnimLabel.h"
#include "ID_Manager.h" 
#include "NineDirection.h"
#include "BrokenLvl.h"
#include "AnimActionEName.h"
#include "GoAltiRange.h"
#include "FloorGoType.h"


#include "RenderLayerType.h"
#include "ShaderType.h"

#include "ColliDataFromJpng.h"



using goLabelId_t = size_t; // std::hash

// 一个 go-action，可以由数个 gomesh-action 组合而成
//
class GoAssemblePlanSet{
public:


    class GoMeshEnt{
    public:
        std::string             goMeshName {}; // 具象go类中 登记的名字
        std::string             animFrameSetName {};
        glm::dvec2              dposOff {}; // gomesh-dposoff based on go-dpos
        double                  zOff    {};
        AnimLabel               animLabel {};
        AnimActionEName         animActionEName {};

        RenderLayerType         renderLayerType {};
        ShaderType              shaderType   {};
        bool                    isVisible {};

        //NineDirection           dir {};
        //BrokenLvl               brokenLvl {};
                                        // dir / broken 数据，是作为 go的数据，从蓝图中被读取的
                                        // 在完善的设计中，每一个 gomesh，都可以选择，直接使用 go的这份数据
                                        // 或者 在 json 文件中，自定义这部分数据
                                        // ...
            
        //------- optional_vals -------//
        std::optional<FloorGoLayer> floorGoLayer    { std::nullopt }; // only for FloorGo   
    };
    

    // 一个 go 实例，由数个 gomesh 以及其他数据 组合而成
    class Plan{
    public:
            
        using id_t = uint32_t;
            
        //---
        Plan()=default;
        std::vector<GoMeshEnt> gomeshs {};

        GoAltiRangeLabel goAltiRangeLabel {};

        std::unique_ptr<ColliDataFromJpng> colliDataFromJpngUPtr {nullptr};

        //------- optional_vals -------//

        //======== static ========//
        static ID_Manager  id_manager;
    };


    //========== Self Vals ==========//
    GoAssemblePlanSet()=default;

    inline Plan &create_new_plan( goLabelId_t labelId_ )noexcept{
        //----- id -----//
        Plan::id_t id = Plan::id_manager.apply_a_u32_id();
        if( labelId_ != DEFAULT_GO_LABEL_ID ){
            auto [insertIt1, insertBool1] = this->ids.insert({ labelId_, std::vector<Plan::id_t>{} }); // insert or find
            insertIt1->second.push_back(id);
        }else{
            // do nothing, goLabelId::Default not belong to any type
        }
        this->allIds.push_back(id); // always
        //----- instance -----//
        auto [insertIt2, insertBool2] = this->planUPtrs.insert({ id, std::make_unique<Plan>() });
        tprAssert( insertBool2 );
        //----- ret -----//
        return *(insertIt2->second);
    }


    inline const Plan &apply_a_plan( goLabelId_t labelId_, size_t randUVal_ )const noexcept{
        //----- id -----//
        size_t          idx {};
        Plan::id_t      id {};  
        if( labelId_ == DEFAULT_GO_LABEL_ID ){
            idx = (randUVal_ + 152375) % this->allIds.size(); // apply a id directly from all ids
            id = this->allIds.at( idx );
        }else{
            tprAssert( this->ids.find(labelId_) != this->ids.end() );
            idx = (randUVal_ + 152375) % this->ids.at(labelId_).size(); // from target idPool
            id = this->ids.at(labelId_).at( idx );
        }
        //--- ret ---//
        tprAssert( this->planUPtrs.find(id) != this->planUPtrs.end() );
        return *(this->planUPtrs.at(id));
    }


    //======= static =======//


    // json 数据中，每个新类型，都被登记为一个 id 号
    inline static void insert_2_goLabelIds( const std::string &str_ )noexcept{
        // 参数表示 默认值时，不需要登记
        if( (str_=="DEFAULT") || (str_=="Default") || (str_=="") ){
            return;
        }
        GoAssemblePlanSet::goLabelIds.insert( GoAssemblePlanSet::hasher(str_) ); // maybe
    }


    // 在程序体内，一律用 id 号来传递 
    static goLabelId_t str_2_goLabelId( const std::string &str_ )noexcept{
        if( (str_=="DEFAULT") || (str_=="Default") || (str_=="") ){ // 表示自己，不为任何类型
            return GoAssemblePlanSet::DEFAULT_GO_LABEL_ID;
        }
        size_t id = GoAssemblePlanSet::hasher(str_);
        tprAssert( GoAssemblePlanSet::goLabelIds.find(id) != GoAssemblePlanSet::goLabelIds.end() );
        return id;
    }
    


private:
    std::unordered_map<goLabelId_t, std::vector<Plan::id_t>> ids {};
    std::vector<Plan::id_t> allIds {};
    //---
    std::unordered_map<Plan::id_t, std::unique_ptr<Plan>> planUPtrs {};


    //======= static =======//
    static std::unordered_set<goLabelId_t> goLabelIds;
                        // 制作成全局容器不够精确，假设某种 go，都含有 goLabel: "Sml"
                        // 另一种go，也想要检测 "Sml" 时，只能获知，总数据中含有，而不是，自身的数据中含有
                        // 先不做修改
                        // ...


    static std::hash<std::string> hasher;
    static const size_t DEFAULT_GO_LABEL_ID;

};


namespace json {//-------- namespace: json --------------//
    void parse_single_goAssemblePlanJsonFile( const std::string &path_file_ );
}//------------- namespace: json end --------------//




#endif 
