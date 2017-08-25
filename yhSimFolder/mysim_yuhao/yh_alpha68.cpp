#include "include/qlSim.h"
namespace
{
class alpha68: public AlphaBase
{
    public:
    explicit alpha68(XMLCONFIG::Element *cfg):
        AlphaBase(cfg), 
        vol(dr.getData<QL_MATRIX<QL_FLOAT>>("volume")), 
        high(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_high")), 
        low(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_low")),
        close(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_close")),
        Num1(cfg->getAttributeIntDefault("para1",8)),
		Num2(cfg->getAttributeIntDefault("para2",13)),
		Num3(cfg->getAttributeIntDefault("para3",1)),
		Num4(cfg->getAttributeIntDefault("para4",15))
	
    {
         adv15.resize (GLOBAL::Dates.size());

        for (int di = 0; di< GLOBAL::Dates.size(); ++di)
        {
            adv15[di].resize(GLOBAL::Instruments.size(),nan);
        }

    }
    void generate(int di) override
    {
        
    vector<float> rank2(GLOBAL::Instruments.size(),nan);


       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {

                vector<float> c1(Num3,nan);
                for (int j = 0; j < Num3; ++ j)
                {
                    c1[j] = vol[di - delay - j][ii];
                }
                adv15[di-delay][ii] = QL_Oputils::mean(c1);
            }
        }
        


        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                rank2[ii] = close[di-delay][ii]*0.518371+low[di-delay][ii]*(1-0.518371)-(close[di-delay-1][ii]*0.518371+low[di-delay-1][ii]*(1-0.518371));
            }
        }        

        QL_Oputils::rank(rank2);

        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
            
                vector<float> c2(Num1,nan);
                vector<float> c3(Num1,nan);
                vector<float> c4(Num2,nan);

            for (int k=0;k<Num2;++k){
            for (int j=0; j<Num1;++j){  
                c3[j] = adv15[di-delay-j-k][ii];
                c2[j] = high[di-delay-j-k][ii];
                
            }
            c4[k]= QL_Oputils::corr(c2,c3);
            }   
            QL_Oputils::rank(c4);



        if (float(c4[0])<float(rank2[ii]))
            {alpha[ii]=-1;}
        else 
            {alpha[ii]=0;}
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

	const QL_MATRIX<QL_FLOAT> &vol;
	const QL_MATRIX<QL_FLOAT> &high;
	const QL_MATRIX<QL_FLOAT> &low;
	const QL_MATRIX<QL_FLOAT> &close;
    vector<vector<float> > adv15;

        int Num1; 
	int Num2;
	int Num3;
	int Num4;
};
}
extern "C"
{
    AlphaBase * createStrategy(XMLCONFIG::Element *cfg)
    {
        AlphaBase * str = new alpha68(cfg);
        return str;
    }
}

