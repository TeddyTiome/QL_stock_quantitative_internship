#include "include/qlSim.h"
namespace
{
class alpha4 : public AlphaBase
{
    public:
    explicit alpha4(XMLCONFIG::Element *cfg):
        AlphaBase(cfg), 
        vol_sum(dr.getData<QL_CUBE<QL_FLOAT>>("intervaldata_5min.vol_sum")),
        last_open(dr.getData<QL_CUBE<QL_FLOAT>>("intervaldata_5min.last_open")),
        last_close(dr.getData<QL_CUBE<QL_FLOAT>>("intervaldata_5min.last_close")),
        vwap(dr.getData<QL_CUBE<QL_FLOAT>>("intervaldata_5min.sum_volume_price")),
        Num1(cfg->getAttributeFloatDefault("para1",0.8))

    {
    }
    void generate(int di) override
    {   
        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {   
            if((valid[ di ][ ii ]))
            {   
                vector<float> smart_scores(48, nan);

                for(int i = 0; i < 48; ++ i)
                {   
                    if (vol_sum(di-delay,i+1,ii) == 0 || last_open(di-delay,i+1,ii) == 0)
                    {
                        smart_scores[i] = 0;
                    }
                    else
                    {
                        float price_change = abs(last_close(di-delay,i+1,ii) - last_open(di-delay,i+1,ii));
                        
                        smart_scores[i] =  price_change * sqrt(sqrt(sqrt(vol_sum(di-delay,i+1,ii))));
                    }
                }

                QL_Oputils::rank(smart_scores);

                float temp1 = 0;
                float temp2 = 0;
                for(int j = 0; j < 48; ++j)
                {
                    if (smart_scores[j] > Num1)
                    {
                        temp1 += vwap(di-delay,j+1,ii) * vol_sum(di-delay,j+1,ii);
                        temp2 += vol_sum(di-delay,j+1,ii);
                    }
                }

                float vwap_smart = nan;
                if (temp2 == 0)
                {
                    vwap_smart = 0;
                }
                else
                {
                    vwap_smart = temp1 / temp2;
                }

                vector<float> c1(48, nan);
                vector<float> c2(48, nan);
                for(int i = 0; i < 48; ++ i)
                {
                    c1[i] = vwap(di-delay,i+1,ii) * vol_sum(di-delay,i+1,ii);
                    c2[i] = vol_sum(di-delay,i+1,ii);
                } 

                if (QL_Oputils::sum(c2) == 0)
                {
                    alpha[ii] = 0;
                }
                else
                {
                    float vwap_all = QL_Oputils::sum(c1) / QL_Oputils::sum(c2);
                    
                    alpha[ii] = - vwap_smart / vwap_all; 
                }
            }
        }

        return;
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

        const QL_CUBE<QL_FLOAT> &vol_sum;
        const QL_CUBE<QL_FLOAT> &last_open;
        const QL_CUBE<QL_FLOAT> &last_close;
        const QL_CUBE<QL_FLOAT> &vwap;
        float Num1;
        
};
}

extern "C"
{
    AlphaBase * createStrategy(XMLCONFIG::Element *cfg)
    {
        AlphaBase * str = new alpha4(cfg);
        return str;
    }
}