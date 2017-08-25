#include "include/qlSim.h"
namespace
{
class alpha27 : public AlphaBase
{
    public:
    explicit alpha27(XMLCONFIG::Element *cfg):
        AlphaBase(cfg),
        volume(dr.getData<QL_MATRIX<QL_FLOAT>>("volume")),
        vwap(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_vwap")),
        Num1(cfg->getAttributeIntDefault("para1",6)),
        Num2(cfg->getAttributeIntDefault("para2",2))

       
    {
    }

    void generate(int di) override
    {
        vector<float> col( GLOBAL::Instruments.size(),nan);
        for (int ii = 0 ;  ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if ((valid[di][ii]))
            {
                vector<float> c1 (Num2,nan);
                for (int i = 0; i< Num2; ++ i)
                {   
                    vector<float> c2(Num1,nan);
                    vector<float> c3(Num1,nan);
                    for (int j = 0; j < Num1 ; ++ j)
                    {
                        c2[j] = volume[di -delay - i -j][ii];
                        c3[j] = vwap[di -delay -i- j][ii];
                    }

                    QL_Oputils::rank(c2);
                    QL_Oputils::rank(c3);
                    c1[i] = QL_Oputils::corr(c2,c3);
                }

                col[ii]= QL_Oputils::sum(c1) / 2;
            }
        }

        QL_Oputils::rank(col);

        for (int ii = 0 ;  ii < GLOBAL::Instruments.size(); ++ ii)
        {
           if ((valid[di][ii]))
           {
            if (col[ii] > 0.5)
            {
                alpha[ii]= -1;
            }
            else
            {
                alpha[ii] = 1;
            }
           } 
        }
        
        return ;
    }




    void checkPointSave(boost::archive::binary_oarchive &ar) 
    {
        ar & *this;
    }
    void checkPointLoad(boost::archive::binary_iarchive &ar)
    {
        ar & *this;
    }
    std::string version() const
    {
        return GLOBALCONST::VERSION;
    }
    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int/*file_version*/)
        {
        }
       
       
        const QL_MATRIX<QL_FLOAT> &volume; 
        const QL_MATRIX<QL_FLOAT> &vwap;
        int Num1;
        int Num2;

     
        
};
}
extern "C"
{
    AlphaBase * createStrategy(XMLCONFIG::Element *cfg)
    {
        AlphaBase * str = new alpha27(cfg);
        return str;
    }
}

